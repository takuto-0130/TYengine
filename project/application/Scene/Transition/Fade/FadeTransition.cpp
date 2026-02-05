#include "FadeTransition.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "../TransitionManager.h"
#include "SpriteBasis.h"
#include <algorithm>

using namespace TYEngine::Utility;
using namespace TYEngine::Core;
using namespace TYEngine::Graphics;

#define FADE_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(FadeTransition, stateEnum, funcName)

const std::vector<StateMachine<FadeTransition, TransitionStage>::StateFunctionSet>& FadeTransition::GetStateTable()
{
	using enum TransitionStage;
	static const std::vector<StateFunctionSet> stateTable = {
		FADE_STATE_ENTRY(IDLE, Idle),
		FADE_STATE_ENTRY(ENTERING, Entering),
		FADE_STATE_ENTRY(EXITING, Exiting)
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
	sprites_ = std::make_unique<TYEngine::Graphics::Sprite>();
	sprites_->Initialize("Resources/Texture/blackScreen.png");
	sprites_->SetSize(Vector2{ WindowsApp::kClientWidth,WindowsApp::kClientHeight });
	sprites_->SetTextureSize(Vector2{ WindowsApp::kClientWidth,WindowsApp::kClientHeight });
}

void FadeTransition::Draw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();
	sprites_->Draw();
}

bool FadeTransition::IsFinished() const
{
	return finished_;
}


void FadeTransition::InitIdle()
{
	sprites_->SetColor(Vector4{ 1,1,1,1 });
	sprites_->Update();
}

void FadeTransition::UpdateIdle()
{
	sprites_->SetColor(Vector4{ 1,1,1,1 });
	sprites_->Update();
}

void FadeTransition::ExitIdle()
{
}

void FadeTransition::InitEntering()
{
	sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
}

void FadeTransition::UpdateEntering()
{
	// フェードイン：不透明(1.0) -> 透明(0.0)
	sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f - std::clamp(GetStateElapsedTime() / duration_, 0.0f, 1.0f) });
	if (GetStateElapsedTime() >= duration_)
	{
		finished_ = true;
		sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	sprites_->Update();
}

void FadeTransition::ExitEntering()
{
}

void FadeTransition::InitExiting()
{
	sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
}

void FadeTransition::UpdateExiting()
{
	// フェードアウト：透明(0.0) -> 不透明(1.0)
	sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, std::clamp(GetStateElapsedTime() / duration_, 0.0f, 1.0f) });
	if (GetStateElapsedTime() >= duration_)
	{
		finished_ = true;
		sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	sprites_->Update();
}

void FadeTransition::ExitExiting()
{
}
