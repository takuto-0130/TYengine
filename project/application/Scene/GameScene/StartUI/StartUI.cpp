#include "StartUI.h"
#include "Sprite.h"
#include "TextureManager.h"

void StartUI::Initialze()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Ready.png");
	ready_ = std::make_unique<Sprite>();
	ready_->Initialize("Resources/Texture/Ready.png");


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Go.png");
	go_ = std::make_unique<Sprite>();
	go_->Initialize("Resources/Texture/Go.png");
}

void StartUI::Update()
{
	ready_->Update();
	go_->Update();
}

void StartUI::Draw()
{
	ready_->Draw();
	go_->Draw();
}
