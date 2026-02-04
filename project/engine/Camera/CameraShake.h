#pragma once
#include "mathFunc.h"
#include <random>

namespace TYEngine
{
	namespace CameraSystem
	{

		/// <summary>
		/// カメラのシェイク（振動）効果を管理するクラス。
		/// パーリンノイズ風のランダム振動を生成し、減衰させる。
		/// </summary>
		class CameraShake
		{
		public:
			// パラメータ
			/// <summary>
			/// シェイク（画面揺れ）のパラメータ構造体。
			/// </summary>
			struct ShakeParams
			{
				float duration = 0.5f;     ///< シェイク継続時間（秒）
				float amplitude = 0.5f;    ///< 揺れの最大強度
				float frequency = 20.0f;   ///< ノイズ周波数（揺れの速さ）
				bool decay = true;         ///< 時間経過で減衰するかどうか
			};

		public:
			CameraShake() = default;

			/// <summary>
			/// シェイクを開始する。
			/// </summary>
			/// <param name="params">シェイク設定パラメータ。</param>
			void Start(const ShakeParams& params)
			{
				params_ = params;
				time_ = 0.0f;
				active_ = true;
			}

			/// <summary>
			/// シェイク状態を更新する。
			/// </summary>
			/// <param name="deltaTime">経過時間。</param>
			void Update(float deltaTime)
			{
				if (!active_) return;

				time_ += deltaTime;
				if (time_ >= params_.duration)
				{
					active_ = false;
					offset_ = { 0, 0, 0 };
					return;
				}

				float progress = time_ / params_.duration;

				float amplitude = params_.decay ? params_.amplitude * (1.0f - progress) : params_.amplitude;
				float t = progress * params_.frequency;

				// 簡易的な疑似乱数振動（周波数を変えてxyzを合成）
				offset_.x = amplitude * sinf(t * 13.0f + RandomOffset(0));
				offset_.y = amplitude * sinf(t * 17.0f + RandomOffset(1));
				offset_.z = amplitude * sinf(t * 11.0f + RandomOffset(2));
			}

			// 現在のオフセットを取得
			const Utility::Vector3& GetOffset() const { return offset_; }

			// アクティブかどうか
			bool IsActive() const { return active_; }

		private:
			float RandomOffset(int seed)
			{
				static std::mt19937 rng(std::random_device{}());
				std::uniform_real_distribution<float> dist(0.0f, 3.14159f * 2.0f);
				rng.seed(seed * 131);
				return dist(rng);
			}

		private:
			ShakeParams params_;
			float time_ = 0.0f;
			bool active_ = false;
			Utility::Vector3 offset_ = {};
		};

	} // namespace Camera
} // namespace TYEngine
