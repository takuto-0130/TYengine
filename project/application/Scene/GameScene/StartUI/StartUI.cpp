#include "StartUI.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"

void StartUI::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Ready.png");
	sprites_[READY] = std::make_unique<Sprite>();
	sprites_[READY]->Initialize("Resources/Texture/Ready.png");
	sprites_[READY]->SetPosition(jm_->Get<Vector2>("StartUI.Texture.Ready.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Go.png");
	sprites_[GO] = std::make_unique<Sprite>();
	sprites_[GO]->Initialize("Resources/Texture/Go.png");
	sprites_[GO]->SetPosition(jm_->Get<Vector2>("StartUI.Texture.Go.Position"));

	timer_ = jm_->Get<float>("RetryUI.timer.reset");
	maxTime_ = jm_->Get<float>("RetryUI.timer.max");
}

void StartUI::DebugJMApply()
{
}

void StartUI::Reset()
{
	sprites_[READY]->SetPosition(jm_->Get<Vector2>("StartUI.Texture.Ready.Position"));
	sprites_[GO]->SetPosition(jm_->Get<Vector2>("StartUI.Texture.Go.Position"));
	timer_ = jm_->Get<float>("RetryUI.timer.reset");
}

void StartUI::Update()
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

void StartUI::Draw()
{
	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}
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
			float x = Lerp(jm_->Get<float>("StartUI.ease.ease1-1.startX"), jm_->Get<float>("StartUI.ease.ease1-1.goalX"), t);
			sprites_[READY]->SetPosition({x, 0});
		}
		else if (timer_ < demiTime * 2.0f)
		{
			// 何もしない（間に処理を入れたければここに）

		}
		else if (timer_ < demiTime * 3.0f)
		{
			float t = (timer_ - demiTime * 2.0f) / demiTime;
			float x = Lerp(jm_->Get<float>("StartUI.ease.ease1-2.startX"), jm_->Get<float>("StartUI.ease.ease1-2.goalX"), t);
			sprites_[READY]->SetPosition({ x, 0 });
			float t1 = (timer_ - demiTime * 2.0f) / demiTime;
			float x1 = Lerp(jm_->Get<float>("StartUI.ease.ease2-1.startX"), jm_->Get<float>("StartUI.ease.ease2-1.goalX"), t1);
			sprites_[GO]->SetPosition({ x1, 0 });
		}
	}
	else
	{
		timer_ = jm_->Get<float>("RetryUI.timer.reset");
	}
}
