#pragma once
#include "Sprite.h"
#include <memory>
class score
{
public:
	void Initialze();

	void Update();

	void Draw();

	struct ScoreDisp {
		std::array<int32_t, 4> num;
	};

	void SetScore(const int32_t& score) 
	{ 
		scoreViewTimer_ = 0;
		prevScore_ = currentScore_;
		currentScore_ = score;
	}

private:
	void ScoreDisplay();

private:
	std::unique_ptr<Sprite> one_;
	std::unique_ptr<Sprite> two_;
	std::unique_ptr<Sprite> three_;
	std::unique_ptr<Sprite> four_;
	std::unique_ptr<Sprite> back_;
	int32_t prevScore_;
	int32_t currentScore_;
	int32_t viewScore_;
	ScoreDisp scoreDisp_;

	float scoreViewTimer_ = 0;
	float kScoreViewTime_ = 1.0f;

	float deltaTime_ = 1.0f / 60.0f;
};

