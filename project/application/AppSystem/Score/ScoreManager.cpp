#include "ScoreManager.h"
#include <algorithm>

void ScoreManager::Init()
{
	jsonManager_ = std::make_unique<TYEngine::Utility::JsonManager>();
	std::string err;
	jsonManager_->Load("ScoreConfig.json", true, &err);

	if (!jsonManager_->Root().contains("basicScore")) {
		jsonManager_->Set("basicScore", 5);
	}
	if (!jsonManager_->Root().contains("maxScore")) {
		jsonManager_->Set("maxScore", 99999);
	}
	jsonManager_->Save();

	// 基礎スコアをセット
	basicScore_ = jsonManager_->Get<int>("basicScore", 5);
	// 最大スコアをセット
	maxScore_ = jsonManager_->Get<int>("maxScore", 99999);
	// スコアを0にリセット
	score_ = 0;
}

void ScoreManager::Update()
{
#ifdef _DEBUG
	basicScore_ = jsonManager_->Get<int>("basicScore", 5);
	maxScore_ = jsonManager_->Get<int>("maxScore", 99999);
#endif
}

void ScoreManager::AddScore(float multiplier)
{
	// 基本スコアに倍率を掛けた値を加算
	score_ += int32_t(float(basicScore_) * multiplier);
	// スコアの上限をkMaxScore_に制限（クランプ）
	score_ = std::clamp<int32_t>(score_, 0, maxScore_);
}
