#include "../GameScene.h"
#include "SceneManager.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../RetryUI/RetryUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "../../Transition/BlockFadeTransition.h"
#include "../../../../engine/Framework/SceneBase/Transition/TransitionManager.h"

using namespace TYEngine::OffScreen;

void GameSceneStateRetry::Init(GameScene& owner)
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(9);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	owner.retryDraw_->Start();
	owner.scoreDraw_->SetResult();
}

void GameSceneStateRetry::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.retryDraw_->Update();
	owner.scoreDraw_->Update();
	if (owner.stateMachine_.GetStateElapsedTime() < 2.5f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((owner.stateMachine_.GetStateElapsedTime() - 0.5f) / 2.0f) * 7.0f);
	}
	else
	{
		if (owner.input_->TriggerKey(DIK_SPACE))
		{
			owner.gameAudio_->Play("enter", false, SoundCategory::UI);
			owner.stateMachine_.ChangeState(GameSceneState::FADE_OUT);
		}
	}
}

void GameSceneStateRetry::Exit(GameScene& owner)
{
	owner.retryDraw_->Reset();
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
}