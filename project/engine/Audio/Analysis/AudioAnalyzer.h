#pragma once
#include <vector>
#include <array>
#include <string>
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

		public:
			/// <summary>コンストラクタ。</summary>
			AudioAnalyzer(const std::string& soundCategory = "");

			/// <summary>
			/// 毎フレームの更新処理。
			/// XAPOから受け取ったデータの解析、スムージングを行う。
			/// </summary>
			void Update();

			/// <summary>
			/// ImGuiによるデバッグ描画。
			/// </summary>
			void Draw();

			/// <summary>解析対象のオーディオハンドルを設定する</summary>
			void SetPlayHandle(int playHandle) { playHandle_ = playHandle; }

			/// <summary>レイテンシ補正オフセット（秒）を設定する</summary>
			void SetLatencyOffset(float offsetSec) { latencyOffsetSec_ = offsetSec; }
			float GetLatencyOffset() const { return latencyOffsetSec_; }

			/// <summary>
			/// 同期されたRMS（音圧）値を取得する。
			/// </summary>
			float GetSyncedRMS() const { return syncedRMS_; }

			/// <summary>
			/// スムージング済みの周波数スペクトラムを取得する。
			/// </summary>
			const std::vector<float>& GetSmoothedSpectrum() const { return spectrumSmoothed_; }

			/// <summary>低音域の強度（0.0～1.0）。</summary>
			float GetLow()  const { return low_; }
			/// <summary>中音域の強度（0.0～1.0）。</summary>
			float GetMid()  const { return mid_; }
			/// <summary>高音域の強度（0.0～1.0）。</summary>
			float GetHigh() const { return high_; }

			/// <summary>最新のSpectral Flux値を取得する。</summary>
			float GetSpectralFlux() const { return latestSpectralFlux_; }

			/// <summary>最新の波形データを取得する。</summary>
			const std::vector<float>& GetWaveform() const { return waveform_; }

			/// <summary>イコライザーの調整用UIを描画する。</summary>
			void DrawEQControl();

		private:
			// 内部処理
			void UpdateRMS();
			void UpdateWaveform();
			void UpdateSpectrumSmoothing();

			// Low/Mid/High 更新
			void UpdateBand();

			void MakeLogSpectrum(
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

			// FFT計算（メインスレッド側）
			void ComputeFFT();
			void AnalyzeBeat();

		private:
			// ---- FFT/解析 関連 ----
			std::vector<float> tempWaveform_; // XAPOからデータを一時抽出するバッファ
			std::vector<float> fftInput_;
			std::vector<float> fftReal_;
			std::vector<float> fftImag_;
			std::vector<float> latestFFT_;
			std::vector<float> prevMag_;
			float latestSpectralFlux_ = 0.0f;

			std::vector<float> logSpectrum_; // 対数スペクトルの一時保存用

			// ---- RMS 関連 ----
			float rmsHistory_[RMS_HISTORY_SIZE] = {};
			int rmsIndex_ = 0;

			float syncedRMS_ = 0.0f;

			std::vector<float> waveform_;  // UI 用の波形
			std::vector<float> waveformScroll_;  // スクロール用のリングバッファ
			int waveformWriteIndex_ = 0;

			// ---- スペクトラム ----
			std::vector<float> spectrumSmoothed_;

			// band値（0〜1）
			float low_ = 0.0f;
			float mid_ = 0.0f;
			float high_ = 0.0f;

			std::string soundCategory_ = "";

			// ---- 同期 関連 ----
			int playHandle_ = -1;
			float latencyOffsetSec_ = 0.08f; // デフォルトのレイテンシ補正（80ms）


		};

	} // namespace Audio
} // namespace TYEngine
