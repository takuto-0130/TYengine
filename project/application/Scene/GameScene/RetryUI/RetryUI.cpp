#include "RetryUI.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"

void RetryUI::Initialze()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Ready.png");
	ready_ = std::make_unique<Sprite>();
	ready_->Initialize("Resources/Texture/Ready.png");
	ready_->SetPosition({ 1280, 0 });

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Go.png");
	go_ = std::make_unique<Sprite>();
	go_->Initialize("Resources/Texture/Go.png");
	go_->SetPosition({ 1280, 0 });

	timer_ = -1.0f;
	maxTime_ = 3.0f;
}

void RetryUI::Reset()
{
}

void RetryUI::Update()
{
	Move();
	ready_->Update();
	go_->Update();
}

void RetryUI::Draw()
{
	ready_->Draw();
	go_->Draw();
}

void RetryUI::Start()
{
	timer_ = 0;
}

void RetryUI::Move()
{
}
