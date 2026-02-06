#include "MyAnalyzerXAPO.h"
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

			energyHistory_.resize(BEAT_ANALYE_BUFFER, 0.0f);
			onsetHistory_.resize(ONSET_HISTORY_SIZE, 0.0f);
		}

		MyAnalyzerXAPO::~MyAnalyzerXAPO() {}

		// --------------------------------------------------------------
		HRESULT __stdcall MyAnalyzerXAPO::LockForProcess(
			UINT32, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* in_params,
			UINT32, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS*)
		{
			const WAVEFORMATEX* fmt = in_params[0].pFormat;

			channels_ = fmt->nChannels;

			// XAudio2 は Submix で float32 に自動変換する
			// ここでは float32 前提で扱う
			return S_OK;
		}

		// --------------------------------------------------------------
		// FFT (高速フーリエ変換) の実行
		// Cooley–Tukey アルゴリズムによる実装
		// --------------------------------------------------------------
		void MyAnalyzerXAPO::ComputeFFT()
		{
			// 入力を実数部にコピー、虚数部は0初期化
			for (UINT32 i = 0; i < FFT_SIZE; i++)
			{
				fftReal_[i] = fftInput_[i];
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
			//// 1. 現在のフレームのエネルギー（RMS）を取得
			//float currentEnergy = latestRMS_;

			//// 2. 過去の平均エネルギーを算出
			//float averageEnergy = 0;
			//for (float e : energyHistory_) averageEnergy += e;
			//averageEnergy /= energyHistory_.size();

			//// 3. 判定（現在の音が平均より特定倍率以上大きければ「拍」）
			//// 係数(1.5fなど)は曲や感度に合わせて調整
			//if (currentEnergy > averageEnergy * 1.5f && !isBeatDetected_)
			//{
			//	// 拍を検出！
			//	isBeatDetected_ = true;
			//	// ここでフラグを立てたり、外部に通知したりする
			//}
			//else if (currentEnergy < averageEnergy)
			//{
			//	isBeatDetected_ = false; // エネルギーが下がったらリセット
			//}

			//// 4. 履歴を更新
			//energyHistory_[energyIndex_] = currentEnergy;
			//energyIndex_ = (energyIndex_ + 1) % energyHistory_.size();



			// 1. 現在の「音の立ち上がり（Onset）」を計算して履歴に保存
			// 前回のRMSとの差分をとることで、音の「出だし」を強調する
			static float lastRMS = 0;
			float onset = max(0.0f, latestRMS_ - lastRMS);
			lastRMS = latestRMS_;

			onsetHistory_[onsetIndex_] = onset;
			onsetIndex_ = (onsetIndex_ + 1) % ONSET_HISTORY_SIZE;

			// --- ここから自己相関の検証 (重いため、数フレームに1回実行するのが理想) ---
			static int skipCount = 0;
			if (++skipCount < 10) return; // 10フレームに1回だけ計算
			skipCount = 0;

			float maxCorrelation = 0;
			int bestLag = 0;

			// ラグ（ズレ）を変えながら計算
			// minLag/maxLag は想定するBPM範囲（60~180BPMなど）に合わせる
			for (int lag = 20; lag < ONSET_HISTORY_SIZE / 2; ++lag)
			{
				float correlation = 0;
				for (int i = 0; i < ONSET_HISTORY_SIZE / 2; ++i)
				{
					int idx1 = (onsetIndex_ - i + ONSET_HISTORY_SIZE) % ONSET_HISTORY_SIZE;
					int idx2 = (onsetIndex_ - i - lag + ONSET_HISTORY_SIZE) % ONSET_HISTORY_SIZE;
					correlation += onsetHistory_[idx1] * onsetHistory_[idx2];
				}

				if (correlation > maxCorrelation)
				{
					maxCorrelation = correlation;
					bestLag = lag;
				}
			}

			// bestLag から BPM を算出（サンプルレートやバッファサイズに依存）
			// 例: 1バッファが約23msなら: BPM = 60 / (bestLag * 0.023)
			if (bestLag > 0)
			{
				estimatedBPM_ = 60.0f / (bestLag * ((float)FFT_SIZE / 44100.0f)); // 概算
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
			if (!enabled) return;

			const float* pIn = (const float*)in->pBuffer;
			float* pOut = (float*)out->pBuffer;
			UINT32 frameCount = in->ValidFrameCount;

			// 波形データのコピー（可視化用）
			int copyCount = min(frameCount, (UINT32)latestWaveform_.size());
			memcpy(latestWaveform_.data(), pIn, sizeof(float) * copyCount);

			// ---- スルー処理 (入力そのまま出力) ----
			if (pIn != pOut) memcpy(pOut, pIn, frameCount * channels_ * sizeof(float));

			if (frameCount == 0) return;

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
