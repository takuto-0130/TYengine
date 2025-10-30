#pragma once
#include <memory>

class Sprite;
class StartUI
{
public:
	void Initialze();

	void Update();

	void Draw();

	void Start();

	void Move();

	void Reset();

private:
	std::unique_ptr<Sprite> ready_;
	std::unique_ptr<Sprite> go_;

	float timer_ = -1.0f;

	float maxTime_ = 3.0f;
};

