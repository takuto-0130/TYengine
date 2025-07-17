#include "ComboManager.h"

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
		comboTimer_ -= 1.0f / 60.0f;

		if (comboTimer_ <= 0) 
		{
			comboTimer_ = 0;
			comboCount_ = 0;
		}
	}
}

void ComboManager::OnEnemyDefeated()
{
	comboCount_++;
	comboTimer_ = kComboTime_;
}
