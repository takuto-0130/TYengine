#include "../GameScene.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"

void GameScene::InitRetry()
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(9);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
}

void GameScene::UpdateRetry()
{
	if (GetStateElapsedTime() < 2.5f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((GetStateElapsedTime() - 0.5f) / 2.0f) * 7.0f);
	}
	else
	{
		if (input_->TriggerKey(DIK_SPACE)) ChangeState(GameSceneState::FADE_OUT);
	}
}

void GameScene::ExitRetry()
{
}