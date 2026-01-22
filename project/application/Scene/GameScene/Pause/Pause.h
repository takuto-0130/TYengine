#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"

class Sprite;
class PauseClass
{
public:
	void Init();

	void Update();

	void Draw();

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void DebugJMApply();

private:
	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;

	jx::JsonManager* jm_;
};

