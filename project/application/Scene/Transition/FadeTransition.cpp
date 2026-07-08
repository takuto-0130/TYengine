#include "FadeTransition.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "../../../engine/Framework/SceneBase/Transition/TransitionManager.h"
#include "SpriteBasis.h"
#include <algorithm>

using namespace TYEngine::Utility;
using namespace TYEngine::Core;
using namespace TYEngine::Graphics;

// マクロ・テーブルの削除

FadeTransition::FadeTransition(FadeTransition::Type type, float duration)
{
	stateMachine_.RegisterState<FadeTransitionStateIdle>(TransitionStage::IDLE, "Idle");
	stateMachine_.RegisterState<FadeTransitionStateEntering>(TransitionStage::ENTERING, "Entering");
	stateMachine_.RegisterState<FadeTransitionStateExiting>(TransitionStage::EXITING, "Exiting");


	duration_ = duration;
	switch (type)
	{
	case Type::IDLE:
		stateMachine_.ChangeState(TransitionStage::IDLE);
		break;
	case Type::FADE_IN:
		stateMachine_.ChangeState(TransitionStage::ENTERING);
		break;
	case Type::FADE_OUT:
		stateMachine_.ChangeState(TransitionStage::EXITING);
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


// --- 状態クラスのメソッド実装 ---
void FadeTransitionStateIdle::Init(FadeTransition& owner)
{
	owner.sprites_->SetColor(Vector4{ 1,1,1,1 });
	owner.sprites_->Update();
}
void FadeTransitionStateIdle::Update(FadeTransition& owner, float)
{
	owner.sprites_->SetColor(Vector4{ 1,1,1,1 });
	owner.sprites_->Update();
}
void FadeTransitionStateIdle::Exit(FadeTransition&) {}

void FadeTransitionStateEntering::Init(FadeTransition& owner)
{
	owner.sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
}
void FadeTransitionStateEntering::Update(FadeTransition& owner, float)
{
	// フェードイン：不透明(1.0) -> 透明(0.0)
	owner.sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f - std::clamp(GetElapsed() / owner.duration_, 0.0f, 1.0f) });
	if (GetElapsed() >= owner.duration_)
	{
		owner.finished_ = true;
		owner.sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
	}
	owner.sprites_->Update();
}
void FadeTransitionStateEntering::Exit(FadeTransition&) {}

void FadeTransitionStateExiting::Init(FadeTransition& owner)
{
	owner.sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
}
void FadeTransitionStateExiting::Update(FadeTransition& owner, float)
{
	// フェードアウト：透明(0.0) -> 不透明(1.0)
	owner.sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, std::clamp(GetElapsed() / owner.duration_, 0.0f, 1.0f) });
	if (GetElapsed() >= owner.duration_)
	{
		owner.finished_ = true;
		owner.sprites_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	owner.sprites_->Update();
}
void FadeTransitionStateExiting::Exit(FadeTransition&) {}
