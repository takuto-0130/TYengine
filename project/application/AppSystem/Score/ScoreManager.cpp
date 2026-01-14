#include "ScoreManager.h"
#include <algorithm>

void ScoreManager::Init()
{
	score_ = 0;
}

void ScoreManager::Update()
{
	// 基準のスコアの変化などを記述する
}

void ScoreManager::AddScore(float multiplier)
{
	score_ += int32_t(float(kBasicScore_) * multiplier);
	score_ = std::clamp<int32_t>(score_, 0, 9999);
}
