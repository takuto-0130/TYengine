#include "ScoreManager.h"

void ScoreManager::Init()
{
	score_ = 0;
}

void ScoreManager::Update()
{
	// 基準のスコアの変化などを記述する
}

void ScoreManager::AddScore(float Multiplyer)
{
	score_ += int32_t(float(kBasicScore_) * Multiplyer);
}
