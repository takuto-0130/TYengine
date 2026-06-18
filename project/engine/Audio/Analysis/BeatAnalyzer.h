#pragma once
#include <string>
#include <vector>

namespace TYEngine
{
	namespace AudioSystem
	{
		// BGMの拍を検出することを想定したクラス
		class BeatAnalyzer
		{
		public:
			/// <summary>コンストラクタ。</summary>
			BeatAnalyzer();

			void Init(const std::string& filename, const std::string& soundCategory = "");
			// BPMを手動で指定 ver
			void Init(float bpm, const std::string& soundCategory);

			/// <summary>
			/// 毎フレームの更新処理。
			/// XAPOから受け取ったデータの解析、スムージングを行う。
			/// </summary>
			void Update();

			void Draw();

			bool GetBeat() { return isBeat_; }

			/// <summary>
			/// 現在の拍との一致率（0.0～1.0）を取得する。
			/// 1.0に近いほど拍のジャストタイミングに近い。
			/// </summary>
			float GetBeatCloseness() const;

			/// <summary>
			/// ジャストタイミング判定（引数で許容範囲を指定）
			/// </summary>
			bool IsJustTiming(float threshold = 0.85f)
			{
				return GetBeatCloseness() >= threshold;
			}

		private:
			void DetectBeat();

		private:
			std::string soundCategory_ = "";

			// ---- ★ビート検出用 ----
			float beatPhase_ = 0.0f;          // 0.0~1.0 (内部メトロノーム)
			bool isBeat_ = false;             // 外部公開用フラグ

			float currentBPM_ = 0.0f;
			float syncCooldown_ = 0.0f; // 同期処理の重複防止用

			static const size_t FLUX_HISTORY_SIZE = 60;
			float fluxHistory_[FLUX_HISTORY_SIZE] = {};
			size_t fluxHistoryIndex_ = 0;
			size_t fluxHistoryCount_ = 0;
		};
	}
}

