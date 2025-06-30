#include "Pause.h"
#include "Sprite.h"
#include "TextureManager.h"

void Pause::Initialze()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/blackScreen.png");
	back_ = std::make_unique<Sprite>();
	back_->Initialize("Resources/Texture/blackScreen.png");
	back_->SetSize(Vector2{ 1280.0f,720.0f });
	back_->SetTextureSize(Vector2{ 1280,720 });
	back_->SetColor(Vector4(1, 1, 1, 0.7f));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/PauseText.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/PauseText.png");
}

void Pause::Update()
{
	back_->Update();
	text_->Update();
}

void Pause::Draw()
{
	back_->Draw();
	text_->Draw();
}
