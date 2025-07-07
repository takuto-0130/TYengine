#include "Pause.h"
#include "Sprite.h"
#include "TextureManager.h"

void PauseClass::Initialze()
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

void PauseClass::Update()
{
	back_->Update();
	text_->Update();
}

void PauseClass::Draw()
{
	back_->Draw();
	text_->Draw();
}
