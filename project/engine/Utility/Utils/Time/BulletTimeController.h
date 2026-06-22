#pragma once
#include "StateMachine.h"
#include "State.h"
#include "SingletonObject.h"
#include <functional>

namespace TYEngine
{
	namespace Utility
	{

		using EaseFunc = std::function<float(float)>;

		class BulletTimeStateNone;
		class BulletTimeStateEnter;
		class BulletTimeStateHold;
		class BulletTimeStateExit;

		/// <summary> バレットタイム（スローモーション）の状態定義。 </summary>
		enum class BulletTimeState
		{
			NONE,	// 通常（等倍速）
			ENTER,	// 補間でスローに入る
			HOLD,	// スロー中（低速維持）
			EXIT,	// 補間で元に戻る（スロー解除）
		};

		class Timer;
		// バレットタイム制御用クラス
		/// <summary>
		/// 時間の流れ（タイムスケール）を制御するクラス。
		/// バレットタイム（スローモーション）への遷移・維持・解除をイージング付きで管理する。
		/// </summary>
		class BulletTimeController
			: public SingletonObject<BulletTimeController>
		{
			friend class SingletonObject<BulletTimeController>;
			friend struct std::default_delete<BulletTimeController>;
			friend class BulletTimeStateNone;
			friend class BulletTimeStateEnter;
			friend class BulletTimeStateHold;
			friend class BulletTimeStateExit;

		private:
			// 外部からの new/delete を禁止
			BulletTimeController();
			~BulletTimeController() = default;

		public:
			using StateMachineType = TYEngine::Utility::StateMachine<BulletTimeState, BulletTimeController>;

		public: // メンバ関数
			/// <summary>
			/// 毎フレームの更新処理。
			/// 状態に応じて TimeManager のタイムスケールを変動させる。
			/// </summary>
			void Update();

			/// <summary>
			/// スローモーションを開始（トリガー）する。
			/// </summary>
			/// <param name="slowScale">目標とするスロー倍率（例：0.1 なら10分の1の速度）。</param>
			/// <param name="enterDuration">設定倍率まで到達するのにかける時間（秒）。</param>
			/// <param name="holdDuration">スローを維持する時間（秒）。</param>
			/// <param name="exitDuration">等倍まで戻す際にかける時間（秒）。</param>
			/// <param name="enterEase">スロー突入時のイージング関数。</param>
			/// <param name="exitEase">スロー解除時のイージング関数。</param>
			void Trigger(float slowScale, float enterDuration, float holdDuration, float exitDuration,
				EaseFunc enterEase, EaseFunc exitEase);
			// ↑今後、開始や終了のトリガーを個別でも設定できるようにする

			/// <summary>
			/// 現在の状態を終了させる（次の状態へ遷移）。
			/// StateMachine の Exit 呼び出しなどで使用。
			/// </summary>
			void CallStateExit();

			/// <summary>
			/// バレットタイムを強制的に中断し、タイムスケールを等倍(1.0)に戻す。
			/// </summary>
			void ForceExitNow();

		private: // メンバ変数
			struct BulletTimeParams
			{

				float slowScale = 0.0f;
				float enterDuration = 0.0f;
				float holdDuration = 0.0f;
				float exitDuration = 0.0f;
				EaseFunc enterEase = nullptr;
				EaseFunc exitEase = nullptr;
			};

			BulletTimeParams params_;
			float elapsed_ = 0.0f;

			// タイマーインスタンスの保持用
			Timer* timer_ = nullptr;

			/// <summary>ステートマシーン。</summary>
			StateMachineType stateMachine_;

		};

		// --- 状態クラスの定義 ---
		class BulletTimeStateNone : public TYEngine::Utility::State<BulletTimeState, BulletTimeController>
		{
		public:
			using State::State;
			void Init(BulletTimeController& owner) override;
			void Update(BulletTimeController& owner, float deltaTime) override;
			void Exit(BulletTimeController& owner) override;
		};

		class BulletTimeStateEnter : public TYEngine::Utility::State<BulletTimeState, BulletTimeController>
		{
		public:
			using State::State;
			void Init(BulletTimeController& owner) override;
			void Update(BulletTimeController& owner, float deltaTime) override;
			void Exit(BulletTimeController& owner) override;
		};

		class BulletTimeStateHold : public TYEngine::Utility::State<BulletTimeState, BulletTimeController>
		{
		public:
			using State::State;
			void Init(BulletTimeController& owner) override;
			void Update(BulletTimeController& owner, float deltaTime) override;
			void Exit(BulletTimeController& owner) override;
		};

		class BulletTimeStateExit : public TYEngine::Utility::State<BulletTimeState, BulletTimeController>
		{
		public:
			using State::State;
			void Init(BulletTimeController& owner) override;
			void Update(BulletTimeController& owner, float deltaTime) override;
			void Exit(BulletTimeController& owner) override;
		};
	} // namespace Utility
} // namespace TYEngine

