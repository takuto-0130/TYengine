#include "StartUI.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"

void StartUI::Initialze()
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

void StartUI::Reset()
{
	go_->SetPosition({ 1280, 0 });
	ready_->SetPosition({ 1280, 0 });
	timer_ = -1.0f;
}

void StartUI::Update()
{
	Move();
	ready_->Update();
	go_->Update();
}

void StartUI::Draw()
{
	ready_->Draw();
	go_->Draw();
}

void StartUI::Start()
{
	timer_ = 0;
}

void StartUI::Move()
{
	if (timer_ >= 0 && timer_ < maxTime_)
	{
		timer_ += Timer::GetInstance()->GetDeltaTime();

		float demiTime = maxTime_ / 3.0f;
		if (timer_ < demiTime)
		{
			float t = timer_ / demiTime;
			float x = Lerp(1280.0f, 0.0f, t);
			ready_->SetPosition({x, 0});
		}
		else if (timer_ < demiTime * 2.0f)
		{

		}
		else if (timer_ < demiTime * 3.0f)
		{
			float t = (timer_ - demiTime * 2.0f) / demiTime;
			float x = Lerp(0.0f, -1280.0f, t);
			ready_->SetPosition({ x, 0 });
			float t1 = (timer_ - demiTime * 2.0f) / demiTime;
			float x1 = Lerp(1280.0f, 0.0f, t1);
			go_->SetPosition({ x1, 0 });
		}
	}
	else
	{
		timer_ = -1.0f;
	}
}
