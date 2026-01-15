#include "ScoreUI.h"
#include "SpriteBasis.h"
#include "TextureManager.h"
#include "mathFunc.h"
#include "Ease.h"
#include "Timer.h"
#include "Random.h"

void ScoreUI::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");

	for (auto&& sprite : sprites_)
	{
		sprite = std::make_unique<Sprite>();
		sprite->Initialize("Resources/Texture/number.png");
		sprite->SetTextureSize(jm_->Get<Vector2>("ScoreUI.Texture.number.TextureSize"));
		sprite->SetSize(jm_->Get<Vector2>("ScoreUI.Texture.number.Size"));
		setColliderSpr_.push_back(sprite.get());
	}
	// オフセット
	OffsetPos(jm_->Get<Vector2>("ScoreUI.Texture.number.offsetPlayPos"));

	scoreViewTime_ = jm_->Get<float>("ScoreUI.ScoreViewTime");
}

void ScoreUI::DebugJMApply()
{
	for (auto&& sprite : sprites_)
	{
		sprite->SetSize(jm_->Get<Vector2>("ScoreUI.Texture.number.Size"));
	}
	OffsetPos(jm_->Get<Vector2>("ScoreUI.Texture.number.offsetPlayPos"));
}

void ScoreUI::OffsetPos(const Vector2& pos)
{
	Vector2 offsetPos = pos;
	float offsetWidth = sprites_[THOUSANDS]->GetSize().x * jm_->Get<float>("ScoreUI.Texture.number.offsetWidthScale");

	for (int i = 0; i < SpriteNum; ++i)
	{
		sprites_[i]->SetPosition({ offsetWidth * i + offsetPos.x, offsetPos.y });
	}
}

void ScoreUI::Update()
{
#ifdef _DEBUG
	DebugJMApply();
#endif // _DEBUG


	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
	ScoreViewSetting();
	for (auto&& sprite : sprites_)
	{
		sprite->Update();
	}
	if (scoreViewTimer_ < scoreViewTime_)
	{
		scoreViewTimer_ += deltaTime_;
		if (scoreViewTimer_ > scoreViewTime_)
		{
			scoreViewTimer_ = scoreViewTime_;
		}
	}
}

void ScoreUI::Draw()
{
	for (auto&& sprite : sprites_)
	{
		sprite->Draw();
	}
}

void ScoreUI::UpdateResult(float currentTime)
{
	(void)currentTime;

	int32_t view = currentScore_;
	scoreDisplay_.num[THOUSANDS] = view / 1000;
	view = view % 1000;

	scoreDisplay_.num[HUNDREDS] = view / 100;
	view = view % 100;

	scoreDisplay_.num[TENS] = view / 10;
	view = view % 10;

	scoreDisplay_.num[ONES] = view;

	if (currentTime < jm_->Get<float>("ScoreUI.ResultTimer.AlphaTimer"))
	{
		currentTime += Timer::GetInstance()->GetDeltaTime();

		float t = currentTime / jm_->Get<float>("ScoreUI.ResultTimer.AlphaTimer");

		for (auto&& sprite : sprites_)
		{
			sprite->SetAlpha(t);
		}
	}

	// タイマーに到達するまで0~9のランダムな数字を描画
	if (currentTime < jm_->Get<float>("ScoreUI.ResultTimer.ConfirmedTimer0"))
	{
		scoreDisplay_.num[THOUSANDS] = Random::GetInstance()->Int09();
	}
	if (currentTime < jm_->Get<float>("ScoreUI.ResultTimer.ConfirmedTimer1"))
	{
		scoreDisplay_.num[HUNDREDS] = Random::GetInstance()->Int09();
	}
	if (currentTime < jm_->Get<float>("ScoreUI.ResultTimer.ConfirmedTimer2"))
	{
		scoreDisplay_.num[TENS] = Random::GetInstance()->Int09();
	}
	if (currentTime < jm_->Get<float>("ScoreUI.ResultTimer.ConfirmedTimer3"))
	{
		scoreDisplay_.num[ONES] = Random::GetInstance()->Int09();
	}

	for (int i = 0; i < SpriteNum; ++i)
	{
		sprites_[i]->SetTextureLeftTop({ sprites_[i]->GetTextureSize().x * scoreDisplay_.num[i],0 });
	}

	for (auto&& sprite : sprites_)
	{
		sprite->Update();
	}
}

void ScoreUI::SetResult()
{
	// オフセット
	OffsetPos(jm_->Get<Vector2>("ScoreUI.Texture.number.offsetResultPos"));
}

void ScoreUI::ScoreViewSetting()
{
	float t = scoreViewTimer_ / scoreViewTime_;
	viewScore_ = int(Lerp(float(prevScore_), float(currentScore_), t));

	int32_t view = viewScore_;
	scoreDisplay_.num[THOUSANDS] = view / 1000;
	view = view % 1000;

	scoreDisplay_.num[HUNDREDS] = view / 100;
	view = view % 100;

	scoreDisplay_.num[TENS] = view / 10;
	view = view % 10;

	scoreDisplay_.num[ONES] = view;

	for (int i = 0; i < SpriteNum; ++i)
	{
		sprites_[i]->SetTextureLeftTop({ sprites_[i]->GetTextureSize().x * scoreDisplay_.num[i],0 });
	}
}
