#include "AudioAnalyzer.h"
#include "Audio.h"
#include "Timer.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace TYEngine
{
	namespace AudioSystem
	{

		AudioAnalyzer::AudioAnalyzer(const std::string& soundCategory)
		{
			spectrumSmoothed_.resize(BANDS, 0.0f);

			waveform_.resize(441);   // 10ms
			waveformScroll_.resize(2000, 0.0f); // 2000 サンプル分のスクロール領域
			waveformWriteIndex_ = 0;

			soundCategory_ = soundCategory;

			// メモリ事前確保
			tempWaveform_.resize(FFT_SIZE, 0.0f);
			fftInput_.resize(FFT_SIZE, 0.0f);
			fftReal_.resize(FFT_SIZE, 0.0f);
			fftImag_.resize(FFT_SIZE, 0.0f);
			latestFFT_.resize(FFT_SIZE, 0.0f);
			prevMag_.resize(FFT_SIZE, 0.0f);
			logSpectrum_.resize(BANDS, 0.0f);
		}

		void AudioAnalyzer::Update()
		{
			auto audio = Audio::GetInstance();
			auto xapo = audio->GetAnalyzerXAPO(soundCategory_);
			if (xapo)
			{
				int sampleRate = xapo->GetSampleRate();
				if (sampleRate == 0) sampleRate = 44100;

				// 再生ハンドルが設定され、再生中の場合は再生位置に同期したサンプルを取得
				if (playHandle_ != -1 && audio->IsPlaying(playHandle_))
				{
					uint64_t samplesPlayed = audio->GetPlaybackSamples(playHandle_);
					int64_t latencySamples = static_cast<int64_t>(latencyOffsetSec_ * sampleRate);
					uint64_t targetSample = (samplesPlayed >= static_cast<uint64_t>(latencySamples)) 
						? (samplesPlayed - latencySamples) 
						: 0;

					xapo->GetWaveformAtSample(targetSample, tempWaveform_, FFT_SIZE);
				}
				else
				{
					// それ以外は最新のデータを取得
					xapo->GetLatestWaveform(tempWaveform_, FFT_SIZE);
				}
				
				// FFT計算
				ComputeFFT();
				
				// Spectral Flux計算
				AnalyzeBeat();
			}

			// RMS（音量）更新
			UpdateRMS();
			// 波形データ更新
			UpdateWaveform();
			// スペクトラムのスムージング処理
			UpdateSpectrumSmoothing();
		}

		void AudioAnalyzer::UpdateRMS()
		{
			// 同期された波形からRMSを計算
			double sum = 0.0;
			for (int i = 0; i < FFT_SIZE; ++i)
			{
				sum += tempWaveform_[i] * tempWaveform_[i];
			}
			float rms = static_cast<float>(std::sqrt(sum / FFT_SIZE));

			// 履歴
			rmsHistory_[rmsIndex_] = rms;
			rmsIndex_ = (rmsIndex_ + 1) % RMS_HISTORY_SIZE;

			// すでに同期された波形から直接計算しているため、このRMSがそのまま同期された値となる
			syncedRMS_ = rms; 
		}

		void AudioAnalyzer::UpdateWaveform()
		{
			// 可視化用の441サンプル（10ms分）を取得
			// tempWaveform_ の末尾441個が最新のデータなので、それをスクロールバッファに追記する
			int N = 441;
			int startOffset = FFT_SIZE - N;

			// リングバッファに書き込む（履歴保存）
			for (int i = 0; i < N; i++)
			{
				waveformScroll_[waveformWriteIndex_] = tempWaveform_[startOffset + i];

				waveformWriteIndex_++;
				if (waveformWriteIndex_ >= waveformScroll_.size())
					waveformWriteIndex_ = 0;
			}

			// 可視用（画面に表示する 441 サンプル分）
			for (int i = 0; i < 441; i++)
			{
				// 常に最新から過去へ遡って取得
				int index = static_cast<int>(waveformWriteIndex_ - (441 - i) + waveformScroll_.size())
					% waveformScroll_.size();

				waveform_[i] = waveformScroll_[index];
			}
		}

		void AudioAnalyzer::ComputeFFT()
		{
			// 窓関数の適用、虚数部は0初期化
			for (int i = 0; i < FFT_SIZE; ++i)
			{
				float w = 0.5f - 0.5f * cosf(2.0f * std::numbers::pi_v<float> * i / (FFT_SIZE - 1));
				fftReal_[i] = tempWaveform_[i] * w; // 窓をかけてからFFTへ
				fftImag_[i] = 0.0f;
			}

			// ビット反転順序への並べ替え
			int j = 0;
			for (int i = 0; i < FFT_SIZE; i++)
			{
				if (i < j)
				{
					std::swap(fftReal_[i], fftReal_[j]);
					std::swap(fftImag_[i], fftImag_[j]);
				}
				int bit = FFT_SIZE >> 1;
				while (j & bit) { j ^= bit; bit >>= 1; }
				j |= bit;
			}

			// バタフライ演算
			for (int len = 2; len <= FFT_SIZE; len <<= 1)
			{
				float ang = -2.0f * std::numbers::pi_v<float> / len;
				float wCos = cosf(ang);
				float wSin = sinf(ang);

				for (int i = 0; i < FFT_SIZE; i += len)
				{
					float uCos = 1.0f;
					float uSin = 0.0f;

					for (int k = 0; k < len / 2; k++)
					{
						int a = i + k;
						int b = i + k + len / 2;

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
			for (int i = 0; i < FFT_SIZE; i++)
				latestFFT_[i] = sqrtf(fftReal_[i] * fftReal_[i] + fftImag_[i] * fftImag_[i]);
		}

		void AudioAnalyzer::AnalyzeBeat()
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

			// 算出した Flux 値を保存
			latestSpectralFlux_ = flux;
		}

		void AudioAnalyzer::UpdateSpectrumSmoothing()
		{
			// 無音判定
			if (syncedRMS_ < 0.00001f)
			{
				for (auto& v : spectrumSmoothed_)
					v = 0.0f;
				return;
			}

			// 同期済みの最新FFT結果からスペクトラム作成
			MakeLogSpectrum(
				Audio::GetInstance()->GetAnalyzerSampleRate(),
				BANDS
			);

			// smoothing
			float decay = 0.90f;
			float attack = 0.70f;

			for (int i = 0; i < BANDS; i++)
			{
				float v = logSpectrum_[i];

				if (v > spectrumSmoothed_[i])
					spectrumSmoothed_[i] = spectrumSmoothed_[i] * (1.0f - attack) + v * attack;
				else
					spectrumSmoothed_[i] *= decay;

				if (spectrumSmoothed_[i] < 0.0001f)
					spectrumSmoothed_[i] = 0.0f;
			}

			UpdateBand();
		}

		void AudioAnalyzer::UpdateBand()
		{
			// 無音なら全部 0
			if (syncedRMS_ < 0.00001f)
			{
				low_ = 0.0f;
				mid_ = 0.0f;
				high_ = 0.0f;
				return;
			}

			// 各バンドの周波数情報を取得
			auto bandsInfo = CalcLogBands(
				BANDS,
				static_cast<float>(Audio::GetInstance()->GetAnalyzerSampleRate())
			);

			// 低域 / 中域 / 高域の集計
			float lowSum = 0.0f; int lowCount = 0;
			float midSum = 0.0f; int midCount = 0;
			float highSum = 0.0f; int highCount = 0;

			// 正規化用。DrawSpectrum では v/2.0f で 0〜1 にしていたので、それに合わせる
			auto normalize = [](float v) -> float
				{
					float n = v / 2.0f;
					if (n < 0.0f) n = 0.0f;
					if (n > 1.0f) n = 1.0f;
					return n;
				};

			const float LOW_MAX = 250.0f;   // 低域上限
			const float MID_MAX = 4000.0f;  // 中域上限

			for (int i = 0; i < BANDS; i++)
			{
				float center = bandsInfo[i].center;
				float v = spectrumSmoothed_[i];

				float n = normalize(v);

				if (center < LOW_MAX)
				{
					lowSum += n;
					lowCount++;
				}
				else if (center < MID_MAX)
				{
					midSum += n;
					midCount++;
				}
				else
				{
					highSum += n;
					highCount++;
				}
			}

			float lowTarget = (lowCount > 0) ? (lowSum / lowCount) : 0.0f;
			float midTarget = (midCount > 0) ? (midSum / midCount) : 0.0f;
			float highTarget = (highCount > 0) ? (highSum / highCount) : 0.0f;

			// 簡易スムージング（攻撃早め / 減衰遅め）
			auto smooth = [](float current, float target) -> float
				{
					const float attack = 0.7f;  // 値が上がるときの追従度
					const float decay = 0.9f;  // 値が下がるときの残り具合

					if (target > current)
						return current * (1.0f - attack) + target * attack;
					else
						return current * decay;
				};

			low_ = smooth(low_, lowTarget);
			mid_ = smooth(mid_, midTarget);
			high_ = smooth(high_, highTarget);
		}

		void AudioAnalyzer::MakeLogSpectrum(
			int sampleRate,
			int bands)
		{
			float minF = 20.0f;              // 人間の可聴最低周波数
			float maxF = sampleRate / 2.0f;  // ナイキスト周波数

			float minLog = log10f(minF);
			float maxLog = log10f(maxF);

			float freqPerBin = (float)sampleRate / FFT_SIZE;

			// 対数スケールで等分割してビンを割り当てる
			for (int b = 0; b < bands; b++)
			{
				float logStart = minLog + (maxLog - minLog) * (float)b / bands;
				float logEnd = minLog + (maxLog - minLog) * (float)(b + 1) / bands;

				float fStart = powf(10.0f, logStart);
				float fEnd = powf(10.0f, logEnd);

				int binStart = (int)(fStart / freqPerBin);
				int binEnd = (int)(fEnd / freqPerBin);

				if (binStart < 0) binStart = 0;
				if (binEnd >= FFT_SIZE / 2) binEnd = FFT_SIZE / 2;

				float sum = 0;
				int count = 0;

				// 該当範囲のFFT値を平均化
				for (int i = binStart; i <= binEnd; i++)
				{
					float v = latestFFT_[i];
					v = log10f(1.0f + v * 1.0f); // 値も対数化してデシベルっぽく
					sum += v;
					count++;
				}

				logSpectrum_[b] = (count > 0) ? sum / count : 0.0f;
			}
		}

		std::vector<AudioAnalyzer::BandInfo> AudioAnalyzer::CalcLogBands(int bands, float sampleRate)
		{
			std::vector<AudioAnalyzer::BandInfo> out(bands);

			float nyquist = sampleRate * 0.5f;

			for (int i = 0; i < bands; i++)
			{
				float t1 = (float)i / (float)bands;
				float t2 = (float)(i + 1) / (float)bands;

				float low = std::pow(10.0f, std::log10(20.0f) + t1 * (std::log10(nyquist) - std::log10(20.0f)));
				float high = std::pow(10.0f, std::log10(20.0f) + t2 * (std::log10(nyquist) - std::log10(20.0f)));
				float center = std::sqrt(low * high);  // 幾何平均

				out[i] = { low, high, center };
			}

			return out;
		}

		void AudioAnalyzer::DrawRSM(float width)
		{
			ImGui::Text("RMS: %.3f", syncedRMS_);
			ImGui::SameLine();
			ImGui::ProgressBar(syncedRMS_, ImVec2(width - 100.0f, 18));
			ImGui::Text("RMS History");

			ImGui::PlotLines("",
				rmsHistory_, RMS_HISTORY_SIZE,
				rmsIndex_, nullptr, 0.0f, 1.0f, ImVec2(width, 80));
		}

		void AudioAnalyzer::DrawSpectrum(float width)
		{
			auto bandsInfo = CalcLogBands(
				BANDS,
				static_cast<float>(Audio::GetInstance()->GetAnalyzerSampleRate())
			);

			float barWidth = width;

			ImGui::Text("SpectrumBar");

			static int selectedBand = -1;

			for (int i = 0; i < BANDS; i++)
			{
				float v = spectrumSmoothed_[i];

				float n = v / 2.0f;
				if (n > 1.0f) n = 1.0f;

				ImVec4 col = (i == selectedBand)
					? ImVec4(1.0f, 0.8f, 0.2f, 1.0f)
					: ImVec4(0.2f, 0.6f, 1.0f, 1.0f);

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);

				ImGui::ProgressBar(n, ImVec2(barWidth, 8), "");

				ImGui::PopStyleColor();

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					selectedBand = i;

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					selectedBand = -1;

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					ImGui::Text("Band %d", i);
					ImGui::Separator();

					float dB = 20.0f * log10f(v + 1e-7f);
					ImGui::Text("Energy: %.1f dB", dB);
					ImGui::Separator();

					if (bandsInfo[i].low >= 1000.0f)
						ImGui::Text("Low:    %.2f kHz", bandsInfo[i].low / 1000.0f);
					else
						ImGui::Text("Low:    %.0f Hz", bandsInfo[i].low);

					if (bandsInfo[i].center >= 1000.0f)
						ImGui::Text("Center:  %.2f kHz", bandsInfo[i].center / 1000.0f);
					else
						ImGui::Text("Center:  %.0f Hz", bandsInfo[i].center);

					if (bandsInfo[i].high >= 1000.0f)
						ImGui::Text("High:   %.2f kHz", bandsInfo[i].high / 1000.0f);
					else
						ImGui::Text("High:   %.0f Hz", bandsInfo[i].high);

					ImGui::EndTooltip();
				}
			}
		}

		void AudioAnalyzer::DrawWaveform(float width)
		{
			ImGui::Text("Waveform");
			ImVec2 size = ImVec2(width, 150);
			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImDrawList* dl = ImGui::GetWindowDrawList();

			dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
				IM_COL32(30, 30, 30, 255));

			const int gridX = 10;
			const int gridY = 6;

			for (int i = 1; i < gridX; i++)
			{
				float x = pos.x + (size.x / gridX) * i;
				dl->AddLine(ImVec2(x, pos.y), ImVec2(x, pos.y + size.y),
					IM_COL32(80, 80, 80, 255));
			}

			for (int i = 1; i < gridY; i++)
			{
				float y = pos.y + (size.y / gridY) * i;
				dl->AddLine(ImVec2(pos.x, y), ImVec2(pos.x + size.x, y),
					IM_COL32(80, 80, 80, 255));
			}

			int N = static_cast<int>(waveform_.size());
			if (N > 1)
			{
				float step = size.x / (float)(N - 1);

				for (int i = 1; i < N; i++)
				{
					float x1 = pos.x + step * (i - 1);
					float x2 = pos.x + step * i;

					float y1 = pos.y + size.y * (0.5f - waveform_[i - 1] * 0.45f);
					float y2 = pos.y + size.y * (0.5f - waveform_[i] * 0.45f);

					dl->AddLine(ImVec2(x1, y1), ImVec2(x2, y2),
						IM_COL32(255, 255, 100, 255), 1.0f);
				}
			}

			ImGui::Dummy(size);
		}

		void AudioAnalyzer::DrawEQControl()
		{
			auto xapo = Audio::GetInstance()->GetAnalyzerXAPO(soundCategory_);
			if (!xapo) return;

			float low = 0.0f, mid = 0.0f, high = 0.0f;
			xapo->GetEQGain(low, mid, high);
			float lp = 0.0f, hp = 0.0f, bp = 0.0f;
			xapo->GetFiltersHz(lp, hp, bp);

			bool changed = false;

			ImGui::Text("Equalizer Settings");
			changed |= ImGui::DragFloat("Low Gain", &low, 0.1f, -15.0f, 15.0f, "%.1f dB");
			changed |= ImGui::DragFloat("Mid Gain", &mid, 0.1f, -15.0f, 15.0f, "%.1f dB");
			changed |= ImGui::DragFloat("High Gain", &high, 0.1f, -15.0f, 15.0f, "%.1f dB");
			changed |= ImGui::DragFloat("LPF Hz", &lp, 10.0f, 1000.0f, 20000.0f, "%.0f Hz");

			if (changed)
			{
				xapo->SetEQGain(low, mid, high);
				xapo->SetFiltersHz(lp, hp, bp);
			}

			// 同期遅延（レイテンシ）調整スライダーを追加
			ImGui::Separator();
			ImGui::Text("Synchronization Settings");
			ImGui::SliderFloat("Latency Offset", &latencyOffsetSec_, 0.0f, 0.5f, "%.3f sec");
		}

		void AudioAnalyzer::Draw()
		{
#ifdef _DEBUG

			ImGui::Begin("Audio Analyzer");

			float width = ImGui::GetWindowWidth() - 15.0f;

			// 音量 (RMS) の可視化
			DrawRSM(width);

			// スペクトラムバーの可視化
			DrawSpectrum(width);

			// 帯域ごとの強度数値表示
			ImGui::Text("Low : %.2f", low_);
			ImGui::Text("Mid : %.2f", mid_);
			ImGui::Text("High : %.2f", high_);

			// イコライザー制御表示
			ImGui::Separator();
			DrawEQControl();
			ImGui::Separator();

			// 波形描画
			DrawWaveform(width);

			ImGui::End();

#endif // _DEBUG
		}

	} // namespace Audio
} // namespace TYEngine
