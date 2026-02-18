#include "Result.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Timer.h"
#include "mathFunc.h"
#include "Ease.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

void ResultClass::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/blackScreen.png");
	sprites_[BLACK] = std::make_unique<Sprite>();
	sprites_[BLACK]->Initialize("Resources/Texture/blackScreen.png");
	sprites_[BLACK]->SetTextureSize(jm_->Get<Vector2>("Result.Texture.blackScreen.TextureSize"));
	sprites_[BLACK]->SetSize(jm_->Get<Vector2>("Result.Texture.blackScreen.Size"));
	sprites_[BLACK]->SetColor(jm_->Get<Vector4>("Result.Texture.blackScreen.Color"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ResultText.png");
	sprites_[RESULT] = std::make_unique<Sprite>();
	sprites_[RESULT]->Initialize("Resources/Texture/ResultText.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ScoreText.png");
	sprites_[SCORE] = std::make_unique<Sprite>();
	sprites_[SCORE]->Initialize("Resources/Texture/ScoreText.png");
	sprites_[SCORE]->SetPosition(jm_->Get<Vector2>("Result.Texture.ScoreText.Position"));
	sprites_[SCORE]->SetAlpha(jm_->Get<float>("Result.Texture.ScoreText.Alpha"));


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleSpace.png");
	sprites_[SPACE] = std::make_unique<Sprite>();
	sprites_[SPACE]->Initialize("Resources/Texture/TitleSpace.png");
	sprites_[SPACE]->SetAnchorPoint(jm_->Get<Vector2>("Result.Texture.TitleSpace.AnchorPoint"));
	sprites_[SPACE]->SetPosition(jm_->Get<Vector2>("Result.Texture.TitleSpace.Position"));
	sprites_[SPACE]->SetAlpha(jm_->Get<float>("Result.Texture.TitleSpace.Alpha"));


	timer_ = jm_->Get<float>("Result.timer.reset");
	maxTime_ = jm_->Get<float>("Result.timer.max");

	setColliderSpr_.push_back(sprites_[RESULT].get());
	setColliderSpr_.push_back(sprites_[SCORE].get());
	setColliderSpr_.push_back(sprites_[SPACE].get());
}

void ResultClass::DebugJMApply()
{
	sprites_[BLACK]->SetTextureSize(jm_->Get<Vector2>("Result.Texture.blackScreen.TextureSize"));
	sprites_[BLACK]->SetSize(jm_->Get<Vector2>("Result.Texture.blackScreen.Size"));
	sprites_[BLACK]->SetColor(jm_->Get<Vector4>("Result.Texture.blackScreen.Color"));

	sprites_[SCORE]->SetPosition(jm_->Get<Vector2>("Result.Texture.ScoreText.Position"));

	sprites_[SPACE]->SetAnchorPoint(jm_->Get<Vector2>("Result.Texture.TitleSpace.AnchorPoint"));
	sprites_[SPACE]->SetPosition(jm_->Get<Vector2>("Result.Texture.TitleSpace.Position"));
}

void ResultClass::Reset()
{
	timer_ = jm_->Get<float>("Result.timer.reset");
}

void ResultClass::Update()
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

void ResultClass::Draw()
{
	for (auto& sprite : sprites_)
	{
		sprite->Draw();
	}
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
		float y = Lerp(jm_->Get<float>("Result.startY"), 0.0f, EaseFixed::OutBounce(t));
		sprites_[RESULT]->SetPosition(jm_->Get<Vector2>("Result.offset") + Vector2{ 0, y });
		sprites_[SCORE]->SetAlpha(t);
		sprites_[SPACE]->SetAlpha(t);
	}
	else
	{
		timer_ = jm_->Get<float>("Result.timer.reset");
	}
}
