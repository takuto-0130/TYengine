#pragma once

namespace TYEngine
{
	namespace Utility
	{
		// 前方宣言
		template<typename StateEnum, typename Owner> class StateMachine;

		/// <summary>
		/// ステートの基底クラス。
		/// 各ステートはこのクラスを継承して、Init/Update/Exitをオーバーライドする。
		/// </summary>
		template<typename StateEnum, typename Owner>
		class State
		{
		public:
			State(StateMachine<StateEnum, Owner>* sm) : sm_(sm) {};
			virtual ~State() = default;

			/// <summary>ステート進入時の初期化処理</summary>
			virtual void Init(Owner&) {}
			/// <summary>ステートの更新処理</summary>
			virtual void Update(Owner&, float) {}
			/// <summary>ステート脱出時の終了処理</summary>
			virtual void Exit(Owner&) {}

			/// <summary>ステート開始からの経過時間を取得</summary>
			float GetElapsed() const { return stateTimer_; }

			/// <summary>ステートマシンを取得</summary>
			StateMachine<StateEnum, Owner>* GetSM() { return sm_; }

		protected:
			/// <summary>ステートマシンに対してステート遷移をリクエストするヘルパー</summary>
			void RequestStateChange(StateEnum next)
			{
				sm_->ChangeState(next);
			}

		private:
			// StateMachineからタイマーを更新できるようにする
			friend class StateMachine<StateEnum, Owner>;
			void TimerUpdate(float deltaTime) { stateTimer_ += deltaTime; }

		private:
			/// <summary>ステートマシン。</summary>
			StateMachine<StateEnum, Owner>* sm_ = nullptr;
			/// <summary>ステート経過時間。</summary>
			float stateTimer_ = 0.0f;
		};
	}
}
