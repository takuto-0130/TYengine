#include "RetryUI.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"
#include "Ease.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

void RetryUI::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/GameOver.png");
	sprites_[GAMEOVER] = std::make_unique<Sprite>();
	sprites_[GAMEOVER]->Initialize("Resources/Texture/GameOver.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ScoreText.png");
	sprites_[SCORE_TEXT] = std::make_unique<Sprite>();
	sprites_[SCORE_TEXT]->Initialize("Resources/Texture/ScoreText.png");
	sprites_[SCORE_TEXT]->SetPosition(jm_->Get<Vector2>("RetryUI.Texture.ScoreText.Position"));
	sprites_[SCORE_TEXT]->SetAlpha(jm_->Get<float>("RetryUI.Texture.ScoreText.Alpha"));


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleSpace.png");
	sprites_[SPACE] = std::make_unique<Sprite>();
	sprites_[SPACE]->Initialize("Resources/Texture/TitleSpace.png");
	sprites_[SPACE]->SetAnchorPoint(jm_->Get<Vector2>("RetryUI.Texture.TitleSpace.AnchorPoint"));
	sprites_[SPACE]->SetPosition(jm_->Get<Vector2>("RetryUI.Texture.TitleSpace.Position"));
	sprites_[SPACE]->SetAlpha(jm_->Get<float>("RetryUI.Texture.TitleSpace.Alpha"));

	timer_ = jm_->Get<float>("RetryUI.timer.reset");
	maxTime_ = jm_->Get<float>("RetryUI.timer.max");
}

void RetryUI::DebugJMApply()
{
	sprites_[SCORE_TEXT]->SetPosition(jm_->Get<Vector2>("RetryUI.Texture.ScoreText.Position"));

	sprites_[SPACE]->SetAnchorPoint(jm_->Get<Vector2>("RetryUI.Texture.TitleSpace.AnchorPoint"));
	sprites_[SPACE]->SetPosition(jm_->Get<Vector2>("RetryUI.Texture.TitleSpace.Position"));
}

void RetryUI::Reset()
{
	timer_ = jm_->Get<float>("RetryUI.timer.reset");
}

void RetryUI::Update()
{
#ifdef _DEBUG
	DebugJMApply();
#endif // _DEBUG

	Move();
	for (auto& sprite : sprites_)
	{
		sprite->Update();
	}
}

void RetryUI::Draw()
{
	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}
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
		float y = Lerp(jm_->Get<float>("RetryUI.ease.startY"), jm_->Get<float>("RetryUI.ease.goalY"), EaseFixed::InBounce(t));
		sprites_[GAMEOVER]->SetPosition({ 0, y });
		sprites_[SCORE_TEXT]->SetAlpha(t);
		sprites_[SPACE]->SetAlpha(t);
	}
	else
	{
		timer_ = jm_->Get<float>("RetryUI.timer.reset");
	}
}
