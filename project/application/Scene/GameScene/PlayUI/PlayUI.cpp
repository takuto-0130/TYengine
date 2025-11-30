#include "PlayUI.h"
#include "Input.h"
#include "mathFunc.h"
#include "TextureManager.h"
#include "../../../ScoreUI/ScoreUI.h"

void PlayUI::Init()
{
	input_ = Input::GetInstance();

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint({ 0.5f,0.5f });

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



	TextureManager::GetInstance()->LoadTexture("Resources/Texture/HpText.png");
	hpText_ = std::make_unique<Sprite>();
	hpText_->Initialize("Resources/Texture/HpText.png");
	hpText_->SetAnchorPoint({ 0.5f,0.5f });
	hpText_->SetPosition(offsetHpTextPos_);

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");
	hpNumTex_ = std::make_unique<Sprite>();
	hpNumTex_->Initialize("Resources/Texture/number.png");
	hpNumTex_->SetTextureSize({ 64,64 });
	hpNumTex_->SetTextureLeftTop({ 128,0 });
	hpNumTex_->SetPosition(offsetHpNum_);
	hpNumTex_->SetSize({ 64,64 });




	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Operation.png");
	operation_ = std::make_unique<Sprite>();
	operation_->Initialize("Resources/Texture/Operation.png");
	operation_->SetSize(operation_->GetSize() * 0.8f);
	operation_->SetPosition({15,145});
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/hightLight.png");
	hightLight_ = std::make_unique<Sprite>();
	hightLight_->Initialize("Resources/Texture/hightLight.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ReturnTitle.png");
	returnTitle_ = std::make_unique<Sprite>();
	returnTitle_->Initialize("Resources/Texture/ReturnTitle.png");
	returnTitle_->SetSize(returnTitle_->GetSize() * 0.8f);
	
}

void PlayUI::Update()
{
	comboText_->Update();
	comboNumTex_->Update();
	operation_->Update();
	hightLight_->Update();
	returnTitle_->Update();

	hpText_->Update();
	hpNumTex_->Update();
}

void PlayUI::Draw()
{
	reticle_->Draw();
	comboNumTex_->Draw();
	comboText_->Draw();
	scoreDraw_->Draw();
//#ifndef _DEBUG
	operation_->Draw();
//#endif // _DEBUG

	hpText_->Draw();
	hpNumTex_->Draw();

}

void PlayUI::DrawHightLight()
{
	hightLight_->Draw();
}

void PlayUI::DrawRT()
{
	reticle_->Update();
	returnTitle_->Draw();
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
