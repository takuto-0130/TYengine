#pragma once
#include <vector>
#include <array>
#include <imgui.h>

namespace TYEngine
{
	namespace AudioSystem
	{

		/// <summary>
		/// オーディオ解析クラス。
		/// 波形データ、FFT解析、RMS（音圧）計測を行い、ビジュアライザリング用のデータを提供する。
		/// </summary>
		class AudioAnalyzer
		{
		public:
			static const int FFT_SIZE = 1024;        ///< FFTサンプル数
			static const int BANDS = 24;              ///< 周波数帯域分割数
			static const int RMS_HISTORY_SIZE = 120; ///< RMS履歴保持数（60fps換算で2秒分）
			static const int DELAY_FRAMES = 5;       ///< 解析遅延フレーム数

		public:
			/// <summary>コンストラクタ。</summary>
			AudioAnalyzer();

			/// <summary>
			/// 毎フレームの更新処理。
			/// XAPOから受け取ったデータの解析、スムージングを行う。
			/// </summary>
			void Update();

			/// <summary>
			/// ImGuiによるデバッグ描画。
			/// </summary>
			void Draw();

			/// <summary>
			/// 同期されたRMS（音圧）値を取得する。
			/// </summary>
			float GetSyncedRMS() const { return syncedRMS_; }

			/// <summary>
			/// スムージング済みの周波数スペクトラムを取得する。
			/// </summary>
			const std::vector<float>& GetSmoothedSpectrum() const { return spectrumSmoothed_; }

			/// <summary>低音域の強度（0.0～1.0）。</summary>
			float GetLowGray()  const { return lowGray_; }
			/// <summary>中音域の強度（0.0～1.0）。</summary>
			float GetMidGray()  const { return midGray_; }
			/// <summary>高音域の強度（0.0～1.0）。</summary>
			float GetHighGray() const { return highGray_; }

		private:
			// 内部処理
			void UpdateRMS();
			void UpdateFFT();
			void UpdateWaveform();
			void UpdateSpectrumSmoothing();

			// Low/Mid/High グレースケール更新
			void UpdateBandGrayscale();

			std::vector<float> MakeLogSpectrum(
				const std::vector<float>& fft,
				int sampleRate,
				int bands);

			struct BandInfo
			{
				float low;
				float high;
				float center;
			};
			std::vector<BandInfo> CalcLogBands(int bands, float sampleRate);

			void DrawRSM(float width);
			void DrawSpectrum(float width);
			void DrawWaveform(float width);

		private:
			// ---- RMS 関連 ----
			float rmsHistory_[RMS_HISTORY_SIZE] = {};
			int rmsIndex_ = 0;

			float rmsDelay_[DELAY_FRAMES] = {};
			int rmsDelayIndex_ = 0;

			float syncedRMS_ = 0.0f;

			// ---- FFT 関連 ----
			std::array<std::array<float, FFT_SIZE>, DELAY_FRAMES> fftDelay_;
			int fftDelayIndex_ = 0;

			std::vector<float> waveform_;  // UI 用の波形
			std::vector<float> waveformScroll_;  // スクロール用のリングバッファ
			int waveformWriteIndex_ = 0;

			// ---- スペクトラム ----
			std::vector<float> spectrumSmoothed_;

			// グレースケール値（0〜1）
			float lowGray_ = 0.0f;
			float midGray_ = 0.0f;
			float highGray_ = 0.0f;

		};

	} // namespace Audio
} // namespace TYEngine
