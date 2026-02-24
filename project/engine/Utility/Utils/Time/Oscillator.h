#pragma once
#include "Timer.h"
#include <cmath>
#include <algorithm>
#include <numbers>

namespace TYEngine
{
	namespace Utility
	{
		/// <summary>
		/// 0.0f ～ 1.0f の間を指定時間で行き来させる計算クラス。
		/// </summary>
		class Oscillator
		{
		public:
			enum class Type
			{
				Sine,   // サイン波（滑らかに行き来する）
				Linear, // 直線的（一定速度で行き来する：ピンポン）
			};

		public:
			/// <summary>
			/// コンストラクタ
			/// </summary>
			/// <param name="cycleDuration">0→1→0 の1往復にかかる時間（秒）</param>
			Oscillator(float cycleDuration = 1.0f, Type type = Type::Sine)
				: duration_(cycleDuration), type_(type)
			{}

			/// <summary>
			/// 時間を進めて現在の値(0.0~1.0)を取得する。
			/// Update内で呼び出してください。
			/// </summary>
			float Update()
			{
				// Timerクラスから DeltaTime を取得
				float dt = Timer::GetInstance()->GetDeltaTime();
				timer_ += dt;

				return GetValue();
			}

			/// <summary>
			/// 計算結果だけを取得する（時間は進めない）
			/// </summary>
			float GetValue() const
			{
				// 0除算防止
				if (duration_ <= 0.0f) return 0.0f;

				float t = timer_;
				float val = 0.0f;

				switch (type_)
				{
				case Type::Sine:
				{
					// Sin波: -1~1 を 0~1 に変換
					// 2PI * (t / duration) で1周
					float sinVal = std::sinf(2.0f * std::numbers::pi_v<float> *(t / duration_));
					// -1~1 -> 0~1 に正規化 ( (sin + 1) / 2 )
					val = (sinVal + 1.0f) * 0.5f;
					break;
				}
				case Type::Linear:
				{
					// ピンポン動作 (0 -> 1 -> 0)
					// 時間を 0 ~ duration_ の範囲に収める
					float cyclePos = std::fmod(t, duration_);

					// 半分の時間 (halfDur) で 1.0 に到達させたい
					float halfDur = duration_ * 0.5f;

					// cyclePos を halfDur で割ると 0.0 ~ 2.0 の値になる
					float normalized = cyclePos / halfDur;

					// 1.0 - abs(normalized - 1.0) で 0 -> 1 -> 0 の三角波(Linear)を作る
					val = 1.0f - std::abs(normalized - 1.0f);
					break;
				}
				}

				return std::clamp(val, 0.0f, 1.0f);
			}

			// パラメータ変更用
			void SetDuration(float duration) { duration_ = duration; }
			void SetType(Type type) { type_ = type; }
			void Reset() { timer_ = 0.0f; }

		private:
			float timer_ = 0.0f;
			float duration_ = 1.0f;
			Type type_ = Type::Sine;
		};
	}
}