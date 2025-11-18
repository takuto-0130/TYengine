#include "ScoreUI.h"
#include "SpriteBasis.h"
#include "TextureManager.h"
#include "mathFunc.h"
#include "Ease.h"
#include "Timer.h"
#include "Random.h"

void ScoreUI::Initialze()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/white2x2.png");
	one_ = std::make_unique<Sprite>();
	one_->Initialize("Resources/Texture/number.png");
	one_->SetTextureSize({ 64,64 });
	one_->SetTextureLeftTop({ 128,0 });
	one_->SetPosition({ 1000,550 });
	one_->SetSize({ 64,64 });

	two_ = std::make_unique<Sprite>();
	two_->Initialize("Resources/Texture/number.png");
	two_->SetTextureSize({ 64,64 });
	two_->SetTextureLeftTop({ 64,0 });
	two_->SetPosition({ 45 + 1000,550 });
	two_->SetSize({ 64,64 });

	three_ = std::make_unique<Sprite>();
	three_->Initialize("Resources/Texture/number.png");
	three_->SetTextureSize({ 64,64 });
	three_->SetTextureLeftTop({ 192,0 });
	three_->SetPosition({ 90 + 1000,550 });
	three_->SetSize({ 64,64 });

	four_ = std::make_unique<Sprite>();
	four_->Initialize("Resources/Texture/number.png");
	four_->SetTextureSize({ 64,64 });
	four_->SetTextureLeftTop({ 0,0 });
	four_->SetPosition({ 135 + 1000,550 });
	four_->SetSize({ 64,64 });

	back_ = std::make_unique<Sprite>();
	back_->Initialize("Resources/Texture/white2x2.png");
	back_->SetSize({ 215,80 });
	back_->SetPosition({ 990,542 });
	back_->SetColor({ 100.0f/256.0f, 160.0f/256.0f, 7.0f/256.0f, 1.0f });


	// 一旦オフセット
	float X = 1020.0f;
	float Y = 620.0f;
	one_->SetPosition({ X,Y });
	two_->SetPosition({ 45.0f + X,Y });
	three_->SetPosition({ 90.0f + X,Y });
	four_->SetPosition({ 135.0f + X,Y });
}

void ScoreUI::Update()
{
	deltaTime_ = Timer::GetInstance()->GetDeltaTime();
	ScoreDisplay();
	one_->Update();
	two_->Update();
	three_->Update();
	four_->Update();
	back_->Update();
	if (scoreViewTimer_ < kScoreViewTime_)
	{
		scoreViewTimer_ += deltaTime_;
		if (scoreViewTimer_ > kScoreViewTime_)
		{
			scoreViewTimer_ = kScoreViewTime_;
		}
	}
}

void ScoreUI::Draw()
{
	//back_->Draw();
	one_->Draw();
	two_->Draw();
	three_->Draw();
	four_->Draw();
}

void ScoreUI::UpdateResult(float currentTime)
{
	(void)currentTime;

	int32_t view = currentScore_;
	scoreDisp_.num[0] = view / 1000;
	view = view % 1000;

	scoreDisp_.num[1] = view / 100;
	view = view % 100;

	scoreDisp_.num[2] = view / 10;
	view = view % 10;

	scoreDisp_.num[3] = view;

	if (currentTime < 2.5f)
	{
		currentTime += Timer::GetInstance()->GetDeltaTime();

		float t = currentTime / 2.5f;

		one_->SetAlpha(t);
		two_->SetAlpha(t);
		three_->SetAlpha(t);
		four_->SetAlpha(t);

		if (currentTime < 1.0f)
		{
			scoreDisp_.num[0] = Random::GetInstance()->Int(0, 9);
		}

		if (currentTime < 1.5f)
		{
			scoreDisp_.num[1] = Random::GetInstance()->Int(0, 9);
		}

		if (currentTime < 2.0f)
		{
			scoreDisp_.num[2] = Random::GetInstance()->Int(0, 9);
		}

		if (currentTime < 2.5f)
		{
			scoreDisp_.num[3] = Random::GetInstance()->Int(0, 9);
		}
	}


	one_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[0],0 });
	two_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[1],0 });
	three_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[2],0 });
	four_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[3],0 });

	one_->Update();
	two_->Update();
	three_->Update();
	four_->Update();
}

void ScoreUI::SetResult()
{
	float X = 650.0f;
	float Y = 360.0f;
	one_->SetPosition({ X,Y });
	two_->SetPosition({ 45.0f + X,Y });
	three_->SetPosition({ 90.0f + X,Y });
	four_->SetPosition({ 135.0f + X,Y });
}

void ScoreUI::ScoreDisplay()
{
	float t = scoreViewTimer_ / kScoreViewTime_;
	viewScore_ = int(Lerp(float(prevScore_), float(currentScore_), t));

	int32_t view = viewScore_;
	scoreDisp_.num[0] = view / 1000;
	view = view % 1000;

	scoreDisp_.num[1] = view / 100;
	view = view % 100;

	scoreDisp_.num[2] = view / 10;
	view = view % 10;

	scoreDisp_.num[3] = view;

	one_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[0],0 });
	two_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[1],0 });
	three_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[2],0 });
	four_->SetTextureLeftTop({ 64.0f * scoreDisp_.num[3],0 });
}
