#include "PlayUI.h"
#include "Input.h"
#include "mathFunc.h"
#include "TextureManager.h"
#include "../../../Score/score.h"

void PlayUI::Init()
{
	input_ = Input::GetInstance();

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint({ 0.5f,0.5f });

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/white2x2.png");
	for (size_t i = 0; i < 2; ++i) {
		lasers_[i] = std::make_unique<Sprite>();
		lasers_[i]->Initialize("Resources/Texture/white2x2.png");
		lasers_[i]->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	}

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ComboText.png");
	comboText_ = std::make_unique<Sprite>();
	comboText_->Initialize("Resources/Texture/ComboText.png");
	comboText_->SetAnchorPoint({ 0.5f,0.5f });
	comboText_->SetPosition(offsetComboTextPos_);

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");
	comboNumTex_ = std::make_unique<Sprite>();
	comboNumTex_->Initialize("Resources/Texture/number.png");
	comboNumTex_->SetTextureSize({ 64,64 });
	comboNumTex_->SetTextureLeftTop({ 128,0 });
	comboNumTex_->SetPosition(offsetComboNum_);
	comboNumTex_->SetSize({ 64,64 });
}

void PlayUI::Update()
{
	comboText_->Update();
	comboNumTex_->Update();
	reticle_->Update();
	for (size_t i = 0; i < 2; ++i) {
		lasers_[i]->Update();
	}
}

void PlayUI::Draw()
{
	Vector2 mouse = input_->GetMousePosition();
	if (input_->PushKey(DIK_SPACE))
	{
		for (size_t i = 0; i < 2; ++i)
		{
			lasers_[i]->DrawRect(mouse, mouse,
				{ 426.7f * float(1 + i) - 20.0f, 720 },
				{ 426.7f * float(1 + i) + 20.0f, 720 });
		}
	}
	reticle_->Draw();
	comboNumTex_->Draw();
	comboText_->Draw();
	scoreDraw_->Draw();
}

void PlayUI::ComboTexUpdate()
{
	Vector2 mouse = input_->GetMousePosition();
	reticle_->SetPosition(mouse);

	float t = comboTimer_ / kComboTime_;
	t = 1.0f - powf(1.0f - t, 4.0f);
	comboText_->SetColor(Vector4(1.0f, 1.0f, 1.0f, t));
	comboNumTex_->SetColor(Vector4(1.0f, 1.0f, 1.0f, t));

	t = (comboTimer_ - (kComboTime_ - shakeTime_)) / (kComboTime_ - (kComboTime_ - shakeTime_));
	if (t > 0)
	{
		std::mt19937 random(seedGene_());
		std::uniform_real_distribution<float> dist(-15.0f, 15.0f);
		Vector2 pos = { dist(random),dist(random) };
		comboText_->SetPosition(offsetComboTextPos_ + pos * t);
		comboNumTex_->SetPosition(offsetComboNum_ + pos * t);
	}
	else
	{
		comboText_->SetPosition(offsetComboTextPos_);
		comboNumTex_->SetPosition(offsetComboNum_);
	}
}
