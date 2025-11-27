#pragma once
#include "Sprite.h"
#include <memory>
class ScoreUI
{
public:
	void Initialze();

	void Update();

	void Draw();

	void UpdateResult(float currentTime);

	struct ScoreDisp {
		std::array<int32_t, 4> num;
	};

	void SetScore(int32_t score) 
	{ 
		scoreViewTimer_ = 0;
		prevScore_ = currentScore_;
		currentScore_ = score;
	}

	// 仮対応
	void SetResult();


	std::vector<Sprite*>& GetSprite() { return setSpr_; }

private:
	void ScoreDisplay();

private:
	std::unique_ptr<Sprite> one_;
	std::unique_ptr<Sprite> two_;
	std::unique_ptr<Sprite> three_;
	std::unique_ptr<Sprite> four_;
	std::unique_ptr<Sprite> back_;
	int32_t prevScore_ = 0;
	int32_t currentScore_ = 0;
	int32_t viewScore_ = 0;
	ScoreDisp scoreDisp_ = {};

	std::vector<Sprite*> setSpr_;

	float scoreViewTimer_ = 0;
	float kScoreViewTime_ = 1.0f;

	float deltaTime_ = 1.0f / 60.0f;
};

