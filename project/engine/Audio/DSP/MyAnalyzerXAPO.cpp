#include "MyAnalyzerXAPO.h"
#include <cmath>
#include <cstring>
#include <numbers>
#include <algorithm>

namespace TYEngine
{
	namespace AudioSystem
	{

		// XAPO registration
		static XAPO_REGISTRATION_PROPERTIES regProps =
		{
			__uuidof(MyAnalyzerXAPO),
			L"MyAnalyzerXAPO",
			L"TYengine",
			1, 0,
			XAPO_FLAG_CHANNELS_MUST_MATCH |
			XAPO_FLAG_FRAMERATE_MUST_MATCH |
			XAPO_FLAG_BUFFERCOUNT_MUST_MATCH |
			XAPO_FLAG_INPLACE_SUPPORTED,
			1, 1,
			1, 1
		};

		// --------------------------------------------------------------
		MyAnalyzerXAPO::MyAnalyzerXAPO()
			: CXAPOParametersBase(&regProps, nullptr, 0, TRUE)
		{
			// リングバッファの初期サイズ確保。十分な大きさ（例: 32768 = 44.1kHz時で約0.74秒分）
			ringBuffer_.resize(32768, 0.0f);
		}

		MyAnalyzerXAPO::~MyAnalyzerXAPO() {}

		// --------------------------------------------------------------
		HRESULT __stdcall MyAnalyzerXAPO::LockForProcess(
			UINT32, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* in_params,
			UINT32, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS*)
		{
			const WAVEFORMATEX* fmt = in_params[0].pFormat;

			channels_ = fmt->nChannels;

			sampleRate_ = fmt->nSamplesPerSec;

			// ここではバンド数=3（Low, Mid, High）として初期化します。
			if (eqFilters_.empty() || eqFilters_[0].size() != channels_)
			{
				eqFilters_.resize(EQBand::BandNum, std::vector<BiquadFilter>(static_cast<size_t>(channels_)));

				UpdateEQ();
			}

			// XAudio2 は Submix で float32 に自动変換する
			// ここでは float32 前提で扱う
			return S_OK;
		}

		void MyAnalyzerXAPO::UpdateEQ()
		{
			// Low(100Hz以下), Mid(1000Hz周辺), High(8000Hz以上)の設定
			for (int ch = 0; ch < static_cast<int>(channels_); ch++)
			{
				eqFilters_[EQBand::LPF][ch].SetCoefficients(
					FilterType::LowPassFilter,
					static_cast<float>(sampleRate_),
					LPHz_.load(std::memory_order_relaxed),
					0.707f);

				eqFilters_[EQBand::HPF][ch].SetCoefficients(
					FilterType::HighPassFilter,
					static_cast<float>(sampleRate_),
					HPHz_.load(std::memory_order_relaxed),
					0.707f);

				eqFilters_[EQBand::BPF][ch].SetCoefficients(
					FilterType::HighPassFilter,
					static_cast<float>(sampleRate_),
					BPHz_.load(std::memory_order_relaxed),
					0.707f);

				eqFilters_[EQBand::Low][ch].SetCoefficients(
					FilterType::LowShelf,
					static_cast<float>(sampleRate_),
					100.0f,
					0.707f,
					lowGain_.load(std::memory_order_relaxed));

				eqFilters_[EQBand::Mid][ch].SetCoefficients(
					FilterType::Peaking,
					static_cast<float>(sampleRate_),
					1000.0f,
					0.707f,
					midGain_.load(std::memory_order_relaxed));

				eqFilters_[EQBand::High][ch].SetCoefficients(
					FilterType::HighShelf,
					static_cast<float>(sampleRate_),
					8000.0f,
					0.707f,
					highGain_.load(std::memory_order_relaxed));
			}
		}

		// --------------------------------------------------------------
		// XAudio2 の音声処理コールバック（リアルタイムスレッドで呼ばれる）
		// --------------------------------------------------------------
		void __stdcall MyAnalyzerXAPO::Process(
			UINT32,
			const XAPO_PROCESS_BUFFER_PARAMETERS* in,
			UINT32,
			XAPO_PROCESS_BUFFER_PARAMETERS* out,
			BOOL enabled)
		{
			const float* pIn = (const float*)in->pBuffer;
			float* pOut = (float*)out->pBuffer;
			UINT32 frameCount = in->ValidFrameCount;

			if (frameCount == 0) return;

			// パラメータ変更があればリアルタイムスレッド側でUpdateEQを実行
			if (parametersChanged_.load(std::memory_order_acquire))
			{
				UpdateEQ();
				parametersChanged_.store(false, std::memory_order_release);
			}

			//  エフェクト無効時やフィルタ未初期化時はスルー出力して安全に終了
			if (!enabled || eqFilters_.empty() || channels_ == 0)
			{
				if (pIn != pOut)
				{
					memcpy(pOut, pIn, frameCount * channels_ * sizeof(float));
				}
				
				// フィルタ無効時でも波形はバッファに入れる
				for (UINT32 i = 0; i < frameCount; ++i)
				{
					float monoSample = 0.0f;
					for (UINT32 ch = 0; ch < channels_; ++ch)
					{
						monoSample += pIn[i * channels_ + ch];
					}
					monoSample /= channels_;

					uint64_t idx = writeIndex_.load(std::memory_order_relaxed);
					ringBuffer_[idx % ringBuffer_.size()] = monoSample;
					writeIndex_.store(idx + 1, std::memory_order_release);
				}
				return;
			}

			bool needFilterReset = false;

			// サンプルごとにフィルタ処理を適用
			for (UINT32 i = 0; i < frameCount; ++i)
			{
				for (UINT32 ch = 0; ch < channels_; ++ch)
				{
					UINT32 index = i * channels_ + ch;
					float sample = pIn[index];

					// 入力に異常値(NaN)が混ざっていたら0にリセット (フィルタ汚染対策)
					if (std::isnan(sample)) sample = 0.0f;

					// フィルタを直列にかける
					sample = eqFilters_[EQBand::Low][ch].Process(sample);
					sample = eqFilters_[EQBand::Mid][ch].Process(sample);
					sample = eqFilters_[EQBand::High][ch].Process(sample);
					sample = eqFilters_[EQBand::LPF][ch].Process(sample);
					//sample = eqFilters_[EQBand::HPF][ch].Process(sample);
					//sample = eqFilters_[EQBand::BPF][ch].Process(sample);

					// フィルタ計算結果が異常値になった場合の検知
					if (std::isnan(sample))
					{
						sample = 0.0f;
						needFilterReset = true;
					}

					// 出力バッファへ書き込み
					pOut[index] = sample;
				}

				// モノラルミックスダウンしてリングバッファへ書き込み
				float monoSample = 0.0f;
				for (UINT32 ch = 0; ch < channels_; ++ch)
				{
					monoSample += pOut[i * channels_ + ch];
				}
				monoSample /= channels_;

				uint64_t idx = writeIndex_.load(std::memory_order_relaxed);
				ringBuffer_[idx % ringBuffer_.size()] = monoSample;
				writeIndex_.store(idx + 1, std::memory_order_release);
			}

			// フィルタがNaN汚染された場合は、BiquadFilterの内部ステートをリセットして復帰させる
			if (needFilterReset)
			{
				for (auto& band : eqFilters_)
				{
					for (auto& f : band)
					{
						f.Reset();
					}
				}
			}
		}

		void MyAnalyzerXAPO::GetLatestWaveform(std::vector<float>& dest, size_t count)
		{
			if (ringBuffer_.empty()) return;

			uint64_t currentWriteIdx = writeIndex_.load(std::memory_order_acquire);
			size_t bufferSize = ringBuffer_.size();

			if (dest.size() < count)
			{
				dest.resize(count);
			}

			for (size_t i = 0; i < count; ++i)
			{
				// 累積インデックス currentWriteIdx から count 分遡って取得
				size_t idx = static_cast<size_t>((currentWriteIdx + bufferSize - count + i) % bufferSize);
				dest[i] = ringBuffer_[idx];
			}
		}

		void MyAnalyzerXAPO::GetWaveformAtSample(uint64_t targetSample, std::vector<float>& dest, size_t count)
		{
			if (ringBuffer_.empty()) return;

			size_t bufferSize = ringBuffer_.size();
			if (dest.size() < count)
			{
				dest.resize(count);
			}

			uint64_t currentWriteIdx = writeIndex_.load(std::memory_order_acquire);

			// targetSampleが未来にある場合は現在の書き込みインデックスにクリップ
			if (targetSample > currentWriteIdx)
			{
				targetSample = currentWriteIdx;
			}
			// targetSampleが過去すぎてバッファの範囲外になった場合は、最も古い利用可能な位置にクリップ
			if (currentWriteIdx > bufferSize && targetSample < currentWriteIdx - bufferSize)
			{
				targetSample = currentWriteIdx - bufferSize + count;
			}

			// targetSample を終端とする範囲の波形データをコピー
			uint64_t startSample = (targetSample >= count) ? (targetSample - count) : 0;

			for (size_t i = 0; i < count; ++i)
			{
				size_t idx = static_cast<size_t>((startSample + i) % bufferSize);
				dest[i] = ringBuffer_[idx];
			}
		}

	} // namespace Audio
} // namespace TYEngine
