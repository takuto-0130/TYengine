#include "ComboManager.h"
#include "Timer.h"

void ComboManager::Init()
{
	comboCount_ = 0;
	comboTimer_ = 0;
	kComboTime_ = 3.0f;
	shakeTime_ = 0.4f;
}

void ComboManager::Update()
{
	if (comboTimer_ > 0)
	{
		comboTimer_ -= Timer::GetInstance()->GetDeltaTime();

		if (comboTimer_ <= 0) 
		{
			comboTimer_ = 0;
			comboCount_ = 0;
		}
	}
}

void ComboManager::OnEnemyDefeated()
{
	if(comboCount_ < 9)
	{
		comboCount_++;
	}
	comboTimer_ = kComboTime_;
}
