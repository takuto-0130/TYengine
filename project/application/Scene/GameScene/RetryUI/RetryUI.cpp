#include "RetryUI.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"
#include "Ease.h"

void RetryUI::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Gameover.png");
	ready_ = std::make_unique<Sprite>();
	ready_->Initialize("Resources/Texture/Gameover.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ScoreText.png");
	scoretext_ = std::make_unique<Sprite>();
	scoretext_->Initialize("Resources/Texture/ScoreText.png");
	scoretext_->SetPosition({ 400, 360 });
	scoretext_->SetAlpha(0.0f);


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleSpace.png");
	spaceSpr_ = std::make_unique<Sprite>();
	spaceSpr_->Initialize("Resources/Texture/TitleSpace.png");
	spaceSpr_->SetAnchorPoint({ 1,1 });
	spaceSpr_->SetPosition({ 1200,700 });
	spaceSpr_->SetAlpha(0.0f);

	timer_ = -1.0f;
	maxTime_ = 2.5f;
}

void RetryUI::Reset()
{
	timer_ = -1.0f;
}

void RetryUI::Update()
{
	Move();
	ready_->Update();
	scoretext_->Update();
	spaceSpr_->Update();
}

void RetryUI::Draw()
{
	ready_->Draw();
	scoretext_->Draw();
	spaceSpr_->Draw();
}

void RetryUI::Start()
{
	timer_ = 0;
}

void RetryUI::Move()
{
	if (timer_ >= 0 && timer_ < maxTime_)
	{
		timer_ += Timer::GetInstance()->GetDeltaTime();

		float t = timer_ / maxTime_;
		float y = Lerp(-300.0f, 0.0f, EaseFixed::InBounce(t));
		ready_->SetPosition({ 0, y });
		scoretext_->SetAlpha(t);
		spaceSpr_->SetAlpha(t);
	}
	else
	{
		timer_ = -1.0f;
	}
}
