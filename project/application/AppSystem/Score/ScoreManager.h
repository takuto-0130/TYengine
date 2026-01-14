#pragma once
#include <cstdint>
class ScoreManager
{
public:
	~ScoreManager() = default;

	void Init();
	void Update();

	void AddScore(float multiplier);

public:
	int32_t GetScore() const { return score_; }
	int32_t GetBasicScore() const { return kBasicScore_; }

private:
	int32_t score_ = 0;
	const int32_t kBasicScore_ = 20;
};

