#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"

class Sprite;
class StartUI
{
public:
	void Init();

	void Update();

	void Draw();

	void Start();

	void Move();

	void Reset();

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void DebugJMApply();

private:
	enum RetryUISprites
	{
		READY,
		GO,
		SpriteNum
	};

	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	float timer_ = 0.0f;

	float maxTime_ = 0.0f;

	jx::JsonManager* jm_;
};

