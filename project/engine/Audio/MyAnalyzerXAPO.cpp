#include "MyAnalyzerXAPO.h"
#include <imgui.h>
#include <cmath>
#include <cstring>
#include <numbers>

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
			latestFFT_.resize(FFT_SIZE, 0.0f);
			latestWaveform_.resize(WAVEFORM_SIZE, 0.0f);

			delayBuffer_.resize(DELAY_FRAMES);
			for (auto& v : delayBuffer_)
				v.resize(FFT_SIZE, 0.0f);

			fftInput_.resize(FFT_SIZE);
			fftReal_.resize(FFT_SIZE);
			fftImag_.resize(FFT_SIZE);

			prevMag_.resize(FFT_SIZE, 0.0f);

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

			// XAudio2 は Submix で float32 に自動変換する
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
					LPHz_,
					0.707f);

				eqFilters_[EQBand::HPF][ch].SetCoefficients(
					FilterType::HighPassFilter,
					static_cast<float>(sampleRate_),
					HPHz_,
					0.707f);

				eqFilters_[EQBand::BPF][ch].SetCoefficients(
					FilterType::HighPassFilter,
					static_cast<float>(sampleRate_),
					BPHz_,
					0.707f);

				eqFilters_[EQBand::Low][ch].SetCoefficients(
					FilterType::LowShelf,
					static_cast<float>(sampleRate_),
					100.0f,
					0.707f,
					lowGain_);

				eqFilters_[EQBand::Mid][ch].SetCoefficients(
					FilterType::Peaking,
					static_cast<float>(sampleRate_),
					1000.0f,
					0.707f,
					midGain_);

				eqFilters_[EQBand::High][ch].SetCoefficients(
					FilterType::HighShelf,
					static_cast<float>(sampleRate_),
					8000.0f,
					0.707f,
					highGain_);
			}
		}

		void MyAnalyzerXAPO::EQImGui()
		{
			float low = lowGain_.load();
			float mid = midGain_.load();
			float high = highGain_.load();
			float lp = LPHz_.load();
			float hp = HPHz_.load();
			float bp = BPHz_.load();

			bool changed = false;

			changed |= ImGui::DragFloat("Low Gain", &low, 0.1f);
			changed |= ImGui::DragFloat("Mid Gain", &mid, 0.1f);
			changed |= ImGui::DragFloat("High Gain", &high, 0.1f);
			changed |= ImGui::DragFloat("LPF Hz", &lp, 10.0f);
			changed |= ImGui::DragFloat("HPF Hz", &hp, 10.0f);
			changed |= ImGui::DragFloat("BPF Hz", &bp, 10.0f);

			if (changed)
			{
				lowGain_.store(low);
				midGain_.store(mid);
				highGain_.store(high);
				LPHz_.store(lp);
				HPHz_.store(hp);
				BPHz_.store(bp);

				UpdateEQ();
			}
		}

		// --------------------------------------------------------------
		// FFT (高速フーリエ変換) の実行
		// Cooley–Tukey アルゴリズムによる実装
		// --------------------------------------------------------------
		void MyAnalyzerXAPO::ComputeFFT()
		{
			// 窓関数の適用、虚数部は0初期化
			for (UINT32 i = 0; i < FFT_SIZE; ++i)
			{
				float w = 0.5f - 0.5f * cosf(2.0f * std::numbers::pi_v<float> * i / (FFT_SIZE - 1));
				fftReal_[i] = fftInput_[i] * w; // 窓をかけてからFFTへ
				fftImag_[i] = 0.0f;
			}

			// ビット反転順序への並べ替え
			UINT32 j = 0;
			for (UINT32 i = 0; i < FFT_SIZE; i++)
			{
				if (i < j)
				{
					std::swap(fftReal_[i], fftReal_[j]);
					std::swap(fftImag_[i], fftImag_[j]);
				}
				UINT32 bit = FFT_SIZE >> 1;
				while (j & bit) { j ^= bit; bit >>= 1; }
				j |= bit;
			}

			// バタフライ演算
			for (UINT32 len = 2; len <= FFT_SIZE; len <<= 1)
			{
				float ang = -2.0f * std::numbers::pi_v<float> / len;
				float wCos = cosf(ang);
				float wSin = sinf(ang);

				for (UINT32 i = 0; i < FFT_SIZE; i += len)
				{
					float uCos = 1.0f;
					float uSin = 0.0f;

					for (UINT32 k = 0; k < len / 2; k++)
					{
						UINT32 a = i + k;
						UINT32 b = i + k + len / 2;

						float xr = fftReal_[b] * uCos - fftImag_[b] * uSin;
						float xi = fftReal_[b] * uSin + fftImag_[b] * uCos;

						fftReal_[b] = fftReal_[a] - xr;
						fftImag_[b] = fftImag_[a] - xi;

						fftReal_[a] += xr;
						fftImag_[a] += xi;

						float ucos2 = uCos * wCos - uSin * wSin;
						uSin = uCos * wSin + uSin * wCos;
						uCos = ucos2;
					}
				}
			}

			// パワースペクトル（振幅）の計算
			for (UINT32 i = 0; i < FFT_SIZE; i++)
				latestFFT_[i] = sqrtf(fftReal_[i] * fftReal_[i] + fftImag_[i] * fftImag_[i]);
		}

		void MyAnalyzerXAPO::AnalyzeBeat()
		{
			float flux = 0.0f;

			// 解析帯域の指定 (FFT_SIZE=1024 の場合、1bin ≒ 43Hz)
			// 2(約86Hz) ～ 60(約2.5kHz) あたりが打楽器の成分
			int start = 2;
			int end = 60;

			for (int i = start; i < end; ++i)
			{
				// 現在の値と前回の値の差分（増加量）を取る
				float diff = latestFFT_[i] - prevMag_[i];

				// 音が大きくなった時だけを足し合わせる (Spectral Flux)
				if (diff > 0.0f)
				{
					flux += diff;
				}

				// 次回のために現在の値を保存
				prevMag_[i] = latestFFT_[i];
			}

			// 算出した Flux 値を保存（これを AudioAnalyzer が GetSpectralFlux で取得する）
			latestSpectralFlux_ = flux;
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

			//  エフェクト無効時やフィルタ未初期化時はスルー出力して安全に終了
			if (!enabled || eqFilters_.empty() || channels_ == 0)
			{
				if (pIn != pOut)
				{
					memcpy(pOut, pIn, frameCount * channels_ * sizeof(float));
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

			// 波形データのコピー（可視化用）
			int copyCount = min(frameCount, (UINT32)latestWaveform_.size());
			memcpy(latestWaveform_.data(), pIn, sizeof(float) * copyCount);

			


			// ---- RMS (二乗平均平方根) 計算 ----
			double sum = 0.0;
			for (UINT32 i = 0; i < frameCount * channels_; i++) sum += pIn[i] * pIn[i];

			latestRMS_ = (float)sqrt(sum / (frameCount * channels_));

			// ---- 無音判定 ----
			if (latestRMS_ < 0.00001f)   // しきい値
			{
				// 無音なら FFT と delayBuffer をクリアして終了
				std::fill(latestFFT_.begin(), latestFFT_.end(), 0.0f);

				for (auto& buf : delayBuffer_) std::fill(buf.begin(), buf.end(), 0.0f);

				std::fill(fftInput_.begin(), fftInput_.end(), 0.0f);

				return;
			}

			// 拍を検出
			AnalyzeBeat();

			// ---- 遅延バッファへの蓄積とFFT実行 ----
			// 今回のサンプルをリングバッファに保存
			UINT32 copy = (frameCount < FFT_SIZE) ? frameCount : FFT_SIZE;

			memset(delayBuffer_[delayIndex_].data(), 0, FFT_SIZE * sizeof(float));
			memcpy(delayBuffer_[delayIndex_].data(), pIn, copy * sizeof(float));

			delayIndex_ = (delayIndex_ + 1) % DELAY_FRAMES;

			// 遅延させたバッファを取り出してFFTにかける
			UINT32 readIndex = (delayIndex_ + DELAY_FRAMES - 1) % DELAY_FRAMES;

			memcpy(fftInput_.data(), delayBuffer_[readIndex].data(), FFT_SIZE * sizeof(float));

			ComputeFFT();
		}

	} // namespace Audio
} // namespace TYEngine
