#pragma once
#include "Sprite.h"

#include "Utils/Json/JsonManager.h"

#include <memory>
#include <random>

class Input;
class ScoreUI;
class PlayUI
{
public:
	void Init();
	void Update();
	void Draw();

	void DrawRT();

	void ComboTexUpdate();

public:
	void SetComboTimer(float timer) { comboTimer_ = timer; }
	void SetComboTime(float time) { kComboTime_ = time; }
	void SetComboNum(int comboNum) { sprites_[COMBO_NUM_TEXT]->SetTextureLeftTop({ 64.0f * float(comboNum),0 }); }
	void SetScoreDraw(ScoreUI* scoreDraw) { scoreDraw_ = scoreDraw; }

	void SetHPNum(int hp = 0) { sprites_[HP_NUM_TEXT]->SetTextureLeftTop({ 64.0f * float(hp),0 }); }

	void SetShiftPos(const Vector2& pos);

	void SetJust(bool just) { isJust_ = just; }

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void DebugJMApply();

private:
	Input* input_ = nullptr;
	ScoreUI* scoreDraw_ = nullptr;

	enum PlayUISprites
	{
		COMBO_TEXT,
		COMBO_NUM_TEXT,
		HP_TEXT,
		HP_NUM_TEXT,
		OPERATION,
		OUTLINE,
		PAUSE,
		SHIFT,
		SpriteNum
	};

	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	std::unique_ptr<Sprite> reticle_;

	float comboTimer_ = 0;
	float kComboTime_ = 0;

	bool isJust_ = false;

	std::random_device seedGene_;
	float shakeTime_ = 0.4f;

	jx::JsonManager* jm_;
};

