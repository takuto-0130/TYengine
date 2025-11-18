#pragma once
#include <memory>
class Sprite;
class ResultClass
{
public:
	void Initialze();

	void Update();

	void Draw();

	void Start();

	void Move();

	void Reset();

private:
	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;
	std::unique_ptr<Sprite> scoretext_;
	std::unique_ptr<Sprite> spaceSpr_;

	float timer_ = -1.0f;

	float maxTime_ = 3.0f;
};

