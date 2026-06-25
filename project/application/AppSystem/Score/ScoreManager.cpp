#include "ScoreManager.h"
#include <algorithm>

void ScoreManager::Init()
{
	// 基礎スコアをセット
	basicScore_ = 20;
	// 最大スコアをセット
	maxScore_ = 99999;
	// スコアを0にリセット
	score_ = 0;
}

void ScoreManager::Update()
{
	// 基準のスコアの変化などを記述する
}

void ScoreManager::AddScore(float multiplier)
{
	// 基本スコアに倍率を掛けた値を加算
	score_ += int32_t(float(basicScore_) * multiplier);
	// スコアの上限をkMaxScore_に制限（クランプ）
	score_ = std::clamp<int32_t>(score_, 0, maxScore_);
}
