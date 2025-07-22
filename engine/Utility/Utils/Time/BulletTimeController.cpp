#include "BulletTimeController.h"
#include "Timer.h"

#include <algorithm>

#define BULLET_TIME_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(BulletTimeController, stateEnum, funcName)

const std::vector<StateMachine<BulletTimeController, BulletTimeState>::StateFunctionSet>& BulletTimeController::GetStateTable()
{
	using enum BulletTimeState;
	static const std::vector<StateFunctionSet> stateTable = {
		BULLET_TIME_ENTRY(NONE, None),
		BULLET_TIME_ENTRY(ENTER, Enter),
		BULLET_TIME_ENTRY(HOLD, Hold),
		BULLET_TIME_ENTRY(EXIT, Exit),
	};
	return stateTable;
}

BulletTimeController::BulletTimeController()
{
	RegisterFromDefaultTable(this);
	timer_ = Timer::GetInstance();
	ChangeState(BulletTimeState::NONE);
}

void BulletTimeController::Update()
{
	UpdateState(timer_->GetRawDeltaTime());
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
	UnlockState();
	ChangeState(BulletTimeState::ENTER);
}

void BulletTimeController::ForceExitNow()
{
	timer_->SetTimeScale(1.0f);
	UnlockState();
	ChangeState(BulletTimeState::NONE);
}



//-----通常-----//
void BulletTimeController::InitNone() 
{
	// スロー終了後に再びトリガーを呼ばないとスローが発生しないように
	LockState();
}
void BulletTimeController::UpdateNone() {}
void BulletTimeController::ExitNone() {}


//-----開始-----//
void BulletTimeController::InitEnter() {}
void BulletTimeController::UpdateEnter()
{
	float t = elapsed_ / params_.enterDuration;
	t = std::clamp(t, 0.0f, 1.0f);

	float easedT = params_.enterEase ? params_.enterEase(t) : t;
	float currentScale = std::lerp(1.0f, params_.slowScale, easedT);
	timer_->SetTimeScale(currentScale);

	elapsed_ += timer_->GetRawDeltaTime();
	if (elapsed_ >= params_.enterDuration)
	{
		timer_->SetTimeScale(params_.slowScale);
		elapsed_ = 0.0f;
		ChangeState(BulletTimeState::HOLD);
	}
}
void BulletTimeController::ExitEnter() {}


//-----維持-----//
void BulletTimeController::InitHold() {}
void BulletTimeController::UpdateHold() 
{
	timer_->SetTimeScale(params_.slowScale);

	elapsed_ += timer_->GetRawDeltaTime();
	if (elapsed_ >= params_.holdDuration) 
	{
		elapsed_ = 0.0f;
		ChangeState(BulletTimeState::EXIT);
	}
}
void BulletTimeController::ExitHold() {}


//-----終了-----//
void BulletTimeController::InitExit() {}
void BulletTimeController::UpdateExit() 
{
	float t = elapsed_ / params_.exitDuration;
	t = std::clamp(t, 0.0f, 1.0f);

	float easedT = params_.exitEase ? params_.exitEase(t) : t;
	float currentScale = std::lerp(params_.slowScale, 1.0f, easedT);
	timer_->SetTimeScale(currentScale);

	elapsed_ += timer_->GetRawDeltaTime();
	if (elapsed_ >= params_.exitDuration) 
	{
		timer_->SetTimeScale(1.0f);
		ChangeState(BulletTimeState::NONE);
	}
}
void BulletTimeController::ExitExit() {}
