#pragma once
#include "Utils/Json/JsonManager.h"

class BaseUI
{
public:
	virtual ~BaseUI() = default;
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void SetJsonManager(TYEngine::Utility::JsonManager* jm) { jm_ = jm; }

protected:
	TYEngine::Utility::JsonManager* jm_ = nullptr;
};
