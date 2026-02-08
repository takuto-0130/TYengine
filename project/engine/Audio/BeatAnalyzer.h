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
			// BPMを手動で指定
			void Init(float bpm, const std::string& soundCategory);

			/// <summary>
			/// 毎フレームの更新処理。
			/// XAPOから受け取ったデータの解析、スムージングを行う。
			/// </summary>
			void Update();

			void Draw();

			bool GetBeat() { return isBeat_; }

		private:
			void DetectBeat();

		private:
			std::string soundCategory_ = "";

			// ---- ★ビート検出用 ----
			float beatPhase_ = 0.0f;          // 0.0~1.0 (内部メトロノーム)
			bool isBeat_ = false;             // 外部公開用フラグ

			float currentBPM_ = 0.0f;
			float syncCooldown_ = 0.0f; // 同期処理の重複防止用

			std::vector<float> fluxHistory_;
		};
	}
}

