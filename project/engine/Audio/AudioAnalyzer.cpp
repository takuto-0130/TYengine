#include "AudioAnalyzer.h"
#include "Audio.h"   // ← 最新RMS / FFT を取得するため

namespace TYEngine
{
	namespace AudioSystem
	{

		AudioAnalyzer::AudioAnalyzer()
		{
			spectrumSmoothed_.resize(BANDS, 0.0f);

			for (auto& f : fftDelay_)
				f.fill(0.0f);

			for (int i = 0; i < DELAY_FRAMES; i++)
				rmsDelay_[i] = 0.0f;

			waveform_.resize(441);   // 10ms
			waveformScroll_.resize(2000, 0.0f); // 2000 サンプル分のスクロール領域
			waveformWriteIndex_ = 0;
		}

		void AudioAnalyzer::Update()
		{
			// RMS（音量）更新 - XAPOから取得
			UpdateRMS();
			// FFT（周波数解析）更新 - XAPOから取得
			UpdateFFT();
			// 波形データ更新
			UpdateWaveform();
			// スペクトラムのスムージング処理
			UpdateSpectrumSmoothing();
		}

		void AudioAnalyzer::UpdateRMS()
		{
			float rms = Audio::GetInstance()->GetAnalyzerXAPO()->GetRMS();

			// 履歴
			rmsHistory_[rmsIndex_] = rms;
			rmsIndex_ = (rmsIndex_ + 1) % RMS_HISTORY_SIZE;

			// 遅延バッファ
			rmsDelay_[rmsDelayIndex_] = rms;
			rmsDelayIndex_ = (rmsDelayIndex_ + 1) % DELAY_FRAMES;

			syncedRMS_ = rmsDelay_[rmsDelayIndex_]; // 遅延後のRMS
		}

		void AudioAnalyzer::UpdateFFT()
		{
			const auto& fftNow = Audio::GetInstance()->GetAnalyzerXAPO()->GetFFT();

			// 遅延バッファにコピー
			for (int i = 0; i < FFT_SIZE; i++)
				fftDelay_[fftDelayIndex_][i] = fftNow[i];

			fftDelayIndex_ = (fftDelayIndex_ + 1) % DELAY_FRAMES;
		}

		void AudioAnalyzer::UpdateWaveform()
		{
			auto* xapo = Audio::GetInstance()->GetAnalyzerXAPO().Get();
			const auto& src = xapo->GetWaveform();

			int N = static_cast<int>(src.size());

			// リングバッファに書き込む（履歴保存）
			for (int i = 0; i < N; i++)
			{
				waveformScroll_[waveformWriteIndex_] = src[i];

				waveformWriteIndex_++;
				if (waveformWriteIndex_ >= waveformScroll_.size())
					waveformWriteIndex_ = 0;
			}

			// 可視用（画面に表示する 441 サンプル分）
			waveform_.resize(441);

			for (int i = 0; i < 441; i++)
			{
				// 常に最新から過去へ遡って取得
				int index = static_cast<int>(waveformWriteIndex_ - (441 - i) + waveformScroll_.size())
					% waveformScroll_.size();

				waveform_[i] = waveformScroll_[index];
			}
		}

		void AudioAnalyzer::UpdateSpectrumSmoothing()
		{
			// 無音判定（RMS と同期させる）
			if (syncedRMS_ < 0.00001f)
			{
				for (auto& v : spectrumSmoothed_)
					v = 0.0f;
				return;
			}

			// 遅延FFTでスペクトラム作成
			const auto& fft = fftDelay_[fftDelayIndex_];

			auto spectrum = MakeLogSpectrum(
				std::vector<float>(fft.begin(), fft.end()),
				Audio::GetInstance()->GetAnalyzerSampleRate(),
				BANDS
			);

			// smoothing
			float decay = 0.90f;
			float attack = 0.70f;

			for (int i = 0; i < BANDS; i++)
			{
				float v = spectrum[i];

				if (v > spectrumSmoothed_[i])
					spectrumSmoothed_[i] = spectrumSmoothed_[i] * (1.0f - attack) + v * attack;
				else
					spectrumSmoothed_[i] *= decay;

				if (spectrumSmoothed_[i] < 0.0001f)
					spectrumSmoothed_[i] = 0.0f;
			}

			UpdateBandGrayscale();
		}

		void AudioAnalyzer::UpdateBandGrayscale()
		{
			// 無音なら全部 0
			if (syncedRMS_ < 0.00001f)
			{
				lowGray_ = 0.0f;
				midGray_ = 0.0f;
				highGray_ = 0.0f;
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

			lowGray_ = smooth(lowGray_, lowTarget);
			midGray_ = smooth(midGray_, midTarget);
			highGray_ = smooth(highGray_, highTarget);
		}

		std::vector<float> AudioAnalyzer::MakeLogSpectrum(
			const std::vector<float>& fft,
			int sampleRate,
			int bands)
		{
			std::vector<float> out(bands, 0.0f);

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
					float v = fft[i];
					v = log10f(1.0f + v * 1.0f); // 値も対数化してデシベルっぽく
					sum += v;
					count++;
				}

				out[b] = (count > 0) ? sum / count : 0.0f;
			}

			return out;
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
			// ----------------------
			// RMS パネル
			// ----------------------
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
			// ----------------------
			// Spectrum パネル
			// ----------------------
			auto bandsInfo = CalcLogBands(
				BANDS,
				static_cast<float>(Audio::GetInstance()->GetAnalyzerSampleRate())
			);

			float barWidth = width;

			ImGui::Text("SpectrumBar");

			// 選択バンド（ハイライト用）
			static int selectedBand = -1;

			for (int i = 0; i < BANDS; i++)
			{
				float v = spectrumSmoothed_[i];

				// normalize
				float n = v / 2.0f;
				if (n > 1.0f) n = 1.0f;

				// ----------------------
				// ハイライト色の設定
				// ----------------------
				ImVec4 col = (i == selectedBand)
					? ImVec4(1.0f, 0.8f, 0.2f, 1.0f)   // 選択時（黄色）
					: ImVec4(0.2f, 0.6f, 1.0f, 1.0f); // 通常（青）

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);

				// バー描画
				ImGui::ProgressBar(n, ImVec2(barWidth, 8), "");

				ImGui::PopStyleColor();


				// ----------------------
				// クリックでハイライト
				// ----------------------
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					selectedBand = i;

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					selectedBand = -1;


				// ----------------------
				// Tooltip（周波数帯 + dB）
				// ----------------------
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();

					ImGui::Text("Band %d", i);
					ImGui::Separator();

					// dB エネルギー
					float dB = 20.0f * log10f(v + 1e-7f);
					ImGui::Text("Energy: %.1f dB", dB);
					ImGui::Separator();

					// Low
					if (bandsInfo[i].low >= 1000.0f)
						ImGui::Text("Low:    %.2f kHz", bandsInfo[i].low / 1000.0f);
					else
						ImGui::Text("Low:    %.0f Hz", bandsInfo[i].low);

					// Center
					if (bandsInfo[i].center >= 1000.0f)
						ImGui::Text("Center:  %.2f kHz", bandsInfo[i].center / 1000.0f);
					else
						ImGui::Text("Center:  %.0f Hz", bandsInfo[i].center);

					// High
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
			// ---------------------------
			// 波形ビューワー
			// ---------------------------

			ImGui::Text("Waveform");
			// --- サイズ指定 ---
			ImVec2 size = ImVec2(width, 150);
			ImVec2 pos = ImGui::GetCursorScreenPos();     // ウィンドウ内の描画開始位置
			ImDrawList* dl = ImGui::GetWindowDrawList();

			// --- 背景 ---
			dl->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y),
				IM_COL32(30, 30, 30, 255));

			// --- グリッド線 ---
			const int gridX = 10;
			const int gridY = 6;

			// 縦線
			for (int i = 1; i < gridX; i++)
			{
				float x = pos.x + (size.x / gridX) * i;
				dl->AddLine(ImVec2(x, pos.y), ImVec2(x, pos.y + size.y),
					IM_COL32(80, 80, 80, 255));
			}

			// 横線
			for (int i = 1; i < gridY; i++)
			{
				float y = pos.y + (size.y / gridY) * i;
				dl->AddLine(ImVec2(pos.x, y), ImVec2(pos.x + size.x, y),
					IM_COL32(80, 80, 80, 255));
			}

			// --- 波形描画 ---
			int N = static_cast<int>(waveform_.size());
			if (N > 1)
			{
				float step = size.x / (float)(N - 1);

				for (int i = 1; i < N; i++)
				{
					float x1 = pos.x + step * (i - 1);
					float x2 = pos.x + step * i;

					// float 波形は -1.0 ～ +1.0 範囲
					float y1 = pos.y + size.y * (0.5f - waveform_[i - 1] * 0.45f);
					float y2 = pos.y + size.y * (0.5f - waveform_[i] * 0.45f);

					dl->AddLine(ImVec2(x1, y1), ImVec2(x2, y2),
						IM_COL32(255, 255, 100, 255), 1.0f);
				}
			}

			ImGui::Dummy(size);
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
			ImGui::Text("Low : %.2f", lowGray_);
			ImGui::Text("Mid : %.2f", midGray_);
			ImGui::Text("High : %.2f", highGray_);

			// 波形描画
			DrawWaveform(width);

			ImGui::End();

#endif // _DEBUG
		}

	} // namespace Audio
} // namespace TYEngine
