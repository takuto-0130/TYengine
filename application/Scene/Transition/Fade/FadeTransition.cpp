#include "FadeTransition.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "../TransitionManager.h"
#include "SpriteBasis.h"
#include <algorithm>

const std::vector<StateMachine<FadeTransition, TransitionStage>::StateFunctionSet>& FadeTransition::GetStateTable()
{
	static const std::vector<StateMachine<FadeTransition, TransitionStage>::StateFunctionSet> stateTable = {
		{ TransitionStage::IDLE,     &FadeTransition::EnterIdle,     &FadeTransition::UpdateIdle,     &FadeTransition::ExitIdle },
		{ TransitionStage::ENTERING, &FadeTransition::EnterEntering, &FadeTransition::UpdateEntering, &FadeTransition::ExitEntering },
		{ TransitionStage::EXITING,  &FadeTransition::EnterExiting,  &FadeTransition::UpdateExiting,  &FadeTransition::ExitExiting }, // ✅ ここ！
	};
	return stateTable;
}

FadeTransition::FadeTransition(FadeTransition::Type type, float duration)
{
	this->template RegisterFromDefaultTable<FadeTransition>(this);


	duration_ = duration;
	switch (type)
	{
	case Type::IDLE:
		ChangeState(TransitionStage::IDLE);
		break;
	case Type::FADE_IN:
		ChangeState(TransitionStage::ENTERING);
		break;
	case Type::FADE_OUT:
		ChangeState(TransitionStage::EXITING);
		break;
	}
}

void FadeTransition::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/blackScreen.png");
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize("Resources/Texture/blackScreen.png");
	sprite_->SetSize(Vector2{ 1280.0f,720.0f });
	sprite_->SetTextureSize(Vector2{ 1280,720 });
}

void FadeTransition::Draw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();
	sprite_->Draw();
}

bool FadeTransition::IsFinished() const
{
	return finished_;
}

void FadeTransition::EnterIdle()
{
	sprite_->SetColor(Vector4{ 1,1,1,1 });
	sprite_->Update();
}

void FadeTransition::UpdateIdle()
{
	sprite_->SetColor(Vector4{ 1,1,1,1 });
	sprite_->Update();
}

void FadeTransition::ExitIdle()
{
}

void FadeTransition::EnterEntering()
{
	sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
}

void FadeTransition::UpdateEntering()
{
	sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f - std::clamp(GetStateElapsedTime() / duration_, 0.0f, 1.0f) });
	if (GetStateElapsedTime() >= duration_)
	{
		finished_ = true;
		sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	sprite_->Update();
}

void FadeTransition::ExitEntering()
{
}

void FadeTransition::EnterExiting()
{
	sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
}

void FadeTransition::UpdateExiting()
{
	sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, std::clamp(GetStateElapsedTime() / duration_, 0.0f, 1.0f) });
	if (GetStateElapsedTime() >= duration_)
	{
		finished_ = true;
		sprite_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	sprite_->Update();
}

void FadeTransition::ExitExiting()
{
}
