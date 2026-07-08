#include "BulletTimeController.h"
#include "Timer.h"

#include <algorithm>

namespace TYEngine
{
	namespace Utility
	{

		BulletTimeController::BulletTimeController()
		{
			stateMachine_.RegisterState<BulletTimeStateNone>(BulletTimeState::NONE, "None");
			stateMachine_.RegisterState<BulletTimeStateEnter>(BulletTimeState::ENTER, "Enter");
			stateMachine_.RegisterState<BulletTimeStateHold>(BulletTimeState::HOLD, "Hold");
			stateMachine_.RegisterState<BulletTimeStateExit>(BulletTimeState::EXIT, "Exit");

			timer_ = Timer::GetInstance();
			stateMachine_.ChangeState(BulletTimeState::NONE);
		}

		void BulletTimeController::Update()
		{
			stateMachine_.UpdateState(*this, timer_->GetRawDeltaTime());
		}

		void BulletTimeController::Trigger(
			float slowScale, float enterDuration, float holdDuration, float exitDuration,
			EaseFunc enterEase, EaseFunc exitEase)
		{
			params_.slowScale = slowScale;
			params_.enterDuration = enterDuration;
			params_.holdDuration = holdDuration;
			params_.exitDuration = exitDuration;
			params_.enterEase = enterEase;
			params_.exitEase = exitEase;

			elapsed_ = 0.0f;
			// 強制的に状態遷移
			stateMachine_.UnlockState();
			stateMachine_.ChangeState(BulletTimeState::ENTER);
		}

		void BulletTimeController::CallStateExit()
		{
			stateMachine_.UnlockState();
			stateMachine_.ChangeState(BulletTimeState::EXIT);
		}

		void BulletTimeController::ForceExitNow()
		{
			// 即時終了
			timer_->SetTimeScale(1.0f);
			stateMachine_.UnlockState();
			stateMachine_.ChangeState(BulletTimeState::NONE);
		}



		//-----通常-----//
		void BulletTimeStateNone::Init(BulletTimeController& owner)
		{
			// スロー終了後に再びトリガーを呼ばないとスローが発生しないように
			owner.stateMachine_.LockState();
		}
		void BulletTimeStateNone::Update(BulletTimeController&, float) {}
		void BulletTimeStateNone::Exit(BulletTimeController&) {}


		//-----開始-----//
		void BulletTimeStateEnter::Init(BulletTimeController&) {}
		void BulletTimeStateEnter::Update(BulletTimeController& owner, float)
		{
			float t = owner.elapsed_ / owner.params_.enterDuration;
			t = std::clamp(t, 0.0f, 1.0f);

			// 徐々にスローへ
			float easedT = owner.params_.enterEase ? owner.params_.enterEase(t) : t;
			float currentScale = std::lerp(1.0f, owner.params_.slowScale, easedT);
			owner.timer_->SetTimeScale(currentScale);

			owner.elapsed_ += owner.timer_->GetRawDeltaTime();
			if (owner.elapsed_ >= owner.params_.enterDuration)
			{
				owner.timer_->SetTimeScale(owner.params_.slowScale);
				owner.elapsed_ = 0.0f;
				owner.stateMachine_.ChangeState(BulletTimeState::HOLD);
			}
		}
		void BulletTimeStateEnter::Exit(BulletTimeController&) {}


		//-----維持-----//
		void BulletTimeStateHold::Init(BulletTimeController&) {}
		void BulletTimeStateHold::Update(BulletTimeController& owner, float)
		{
			owner.timer_->SetTimeScale(owner.params_.slowScale);

			owner.elapsed_ += owner.timer_->GetRawDeltaTime();
			if (owner.elapsed_ >= owner.params_.holdDuration)
			{
				owner.elapsed_ = 0.0f;
				// 維持時間終了、Exitへ
				owner.stateMachine_.ChangeState(BulletTimeState::EXIT);
			}
		}
		void BulletTimeStateHold::Exit(BulletTimeController&) {}


		//-----終了-----//
		void BulletTimeStateExit::Init(BulletTimeController&) {}
		void BulletTimeStateExit::Update(BulletTimeController& owner, float)
		{
			float t = owner.elapsed_ / owner.params_.exitDuration;
			t = std::clamp(t, 0.0f, 1.0f);

			// 徐々に通常速度へ
			float easedT = owner.params_.exitEase ? owner.params_.exitEase(t) : t;
			float currentScale = std::lerp(owner.params_.slowScale, 1.0f, easedT);
			owner.timer_->SetTimeScale(currentScale);

			owner.elapsed_ += owner.timer_->GetRawDeltaTime();
			if (owner.elapsed_ >= owner.params_.exitDuration)
			{
				owner.timer_->SetTimeScale(1.0f);
				owner.stateMachine_.ChangeState(BulletTimeState::NONE);
			}
		}
		void BulletTimeStateExit::Exit(BulletTimeController&) {}

	} // namespace Utility
} // namespace TYEngine
