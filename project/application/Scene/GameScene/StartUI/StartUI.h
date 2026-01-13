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
	std::unique_ptr<Sprite> ready_;
	std::unique_ptr<Sprite> go_;

	float timer_ = -1.0f;

	float maxTime_ = 3.0f;

	jx::JsonManager* jm_;
};

