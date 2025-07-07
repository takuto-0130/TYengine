#pragma once
#include "Sprite.h"

#include <memory>
#include <random>

class Input;
class Score;
class PlayUI
{
public:
	void Init() ;
	void Update();
	void Draw();

	void ComboTexUpdate();

public:
	void SetComboTimer(float timer) { comboTimer_ = timer; }
	void SetComboTime(float time) { kComboTime_ = time; }
	void SetComboNum(int comboNum) { comboNumTex_->SetTextureLeftTop({ 64.0f * float(comboNum),0 }); }
	void SetScoreDraw(Score* scoreDraw) { scoreDraw_ = scoreDraw; }

private:
	Input* input_ = nullptr;
	Score* scoreDraw_ = nullptr;

	std::unique_ptr<Sprite> comboNumTex_;
	Vector2 offsetComboNum_ = { 1060,25 };
	std::unique_ptr<Sprite> comboText_;
	Vector2 offsetComboTextPos_ = { 1245, 60 };

	std::unique_ptr<Sprite> reticle_;
	std::array<std::unique_ptr<Sprite>, 2> lasers_;


	float comboTimer_ = 0;
	float kComboTime_ = 0;

	std::random_device seedGene_;
	float shakeTime_ = 0.4f;
};

