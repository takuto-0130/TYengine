#include "../GameScene.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../Result/Result.h"
#include "../../../ScoreUI/ScoreUI.h"

void GameScene::InitResult()
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(9);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	resultMenu_->Start();
	scoreDraw_->SetResult();
	scoreDraw_->UpdateResult(0.0f);
}
void GameScene::UpdateResult()
{
	resultMenu_->Update();
	scoreDraw_->UpdateResult(GetStateElapsedTime());

	if (GetStateElapsedTime() < 2.5f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((GetStateElapsedTime() - 0.5f) / 2.0f) * 7.0f);
	}
	else
	{
		if (input_->TriggerKey(DIK_SPACE)) ChangeState(GameSceneState::FADE_OUT);
	}
}
void GameScene::ExitResult()
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	resultMenu_->Reset();
}