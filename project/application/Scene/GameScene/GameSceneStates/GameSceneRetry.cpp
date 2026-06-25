#include "../GameScene.h"
#include "SceneManager.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../RetryUI/RetryUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "../../Transition/BlockFadeTransition.h"
#include "../../../../engine/Framework/SceneBase/Transition/TransitionManager.h"
#include "UIManager.h"

using namespace TYEngine::OffScreen;

void GameSceneStateRetry::Init(GameScene& owner)
{
	(void)owner;
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(9);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	
	auto* uiMgr = UIManager::GetInstance();
	if (auto* retry = uiMgr->GetUI<RetryUI>("Retry"))
	{
		retry->Start();
	}
	if (auto* score = uiMgr->GetUI<ScoreUI>("Score"))
	{
		score->SetResult();
	}
}

void GameSceneStateRetry::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	auto* uiMgr = UIManager::GetInstance();
	uiMgr->UpdateUI("Retry");
	uiMgr->UpdateUI("Score");
	
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
	(void)owner;
	if (auto* retry = UIManager::GetInstance()->GetUI<RetryUI>("Retry"))
	{
		retry->Reset();
	}
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
}