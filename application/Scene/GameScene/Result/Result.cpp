#include "Result.h"
#include "Sprite.h"
#include "TextureManager.h"

void Result::Initialze()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/blackScreen.png");
	back_ = std::make_unique<Sprite>();
	back_->Initialize("Resources/Texture/blackScreen.png");
	back_->SetSize(Vector2{ 1280.0f,720.0f });
	back_->SetTextureSize(Vector2{ 1280,720 });
	back_->SetColor(Vector4(1, 1, 1, 0.7f));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ResultText.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/ResultText.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ScoreText.png");
	scoretext_ = std::make_unique<Sprite>();
	scoretext_->Initialize("Resources/Texture/ScoreText.png");
	scoretext_->SetPosition({ 450, 360 });


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleSpace.png");
	spaceSpr_ = std::make_unique<Sprite>();
	spaceSpr_->Initialize("Resources/Texture/TitleSpace.png");
	spaceSpr_->SetAnchorPoint({ 1,1 });
	spaceSpr_->SetPosition({ 1200,700 });
}

void Result::Update()
{
	back_->Update();
	text_->Update();
	scoretext_->Update();
	spaceSpr_->Update();
}

void Result::Draw()
{
	back_->Draw();
	text_->Draw();
	scoretext_->Draw();
	spaceSpr_->Draw();
}
