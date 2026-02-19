#include "../GameScene.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../RetryUI/RetryUI.h"
#include "../../../ScoreUI/ScoreUI.h"


using namespace TYEngine::OffScreen;

void GameScene::InitRetry()
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(9);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	retryDraw_->Start();
	scoreDraw_->SetResult();
}

void GameScene::UpdateRetry()
{
	retryDraw_->Update();
	scoreDraw_->Update();
	if (stateMachine_.GetStateElapsedTime() < 2.5f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((stateMachine_.GetStateElapsedTime() - 0.5f) / 2.0f) * 7.0f);
	}
	else
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			gameAudio_->Play("enter", false, SoundCategory::UI);
			stateMachine_.ChangeState(GameSceneState::FADE_OUT);
		}
	}
}

void GameScene::ExitRetry()
{
	retryDraw_->Reset();
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
}