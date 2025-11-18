#include "Result.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"
#include "Ease.h"

void ResultClass::Initialze()
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

void ResultClass::Reset()
{
	timer_ = -1.0f;
}

void ResultClass::Update()
{
	//back_->Update();
	Move();
	text_->Update();
	scoretext_->Update();
	spaceSpr_->Update();
}

void ResultClass::Draw()
{
	back_->Draw();
	text_->Draw();
	scoretext_->Draw();
	spaceSpr_->Draw();
}

void ResultClass::Start()
{
	timer_ = 0;
}

void ResultClass::Move()
{
	if (timer_ >= 0 && timer_ < maxTime_)
	{
		timer_ += Timer::GetInstance()->GetDeltaTime();

		float t = timer_ / maxTime_;
		float y = Lerp(-300.0f, 0.0f, EaseFixed::InBounce(t));
		text_->SetPosition({ 510, y + 230 });
		scoretext_->SetAlpha(t);
		spaceSpr_->SetAlpha(t);
	}
	else
	{
		timer_ = -1.0f;
	}
}
