#pragma once
#include <memory>
#include "Sprite/Sprite.h"

#include "Utils/Json/JsonManager.h"

class ResultClass
{
public:
	void Init();

	void Update();

	void Draw();

	void Start();

	void Move();

	void Reset();

	std::vector<Sprite*>& GetSprite() { return setColliderSpr_; }

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void DebugJMApply();

private:
	enum ResultUISprites
	{
		BLACK,
		RESULT,
		SCORE,
		SPACE,
		RETURN_TITLE,
		SpriteNum
	};

	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	std::vector<Sprite*> setColliderSpr_;

	float timer_ = 0.0f;

	float maxTime_ = 0.0f;

	jx::JsonManager* jm_;
};

