#include "../GameScene.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../Result/Result.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "Timer.h"

void GameScene::InitResult()
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(13);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	resultMenu_->Start();
	scoreDraw_->SetResult();
	scoreDraw_->UpdateResult(0.0f);

	std::vector<Sprite*> setSpr;
	setSpr.insert(setSpr.end(), resultMenu_->GetSprite().begin(), resultMenu_->GetSprite().end());
	setSpr.insert(setSpr.end(), scoreDraw_->GetSprite().begin(), scoreDraw_->GetSprite().end());

	uiCollider_.SetSprites(setSpr);

	confetti_.Init(
		400,
		"Resources/Texture/white2x2.png",
		WindowsApp::kClientWidth,
		WindowsApp::kClientHieght
	);

	confetti_.SetUIColliders(uiCollider_.GetAABBs());

	// リザルト突入と同時に一気に出す
	confetti_.Burst(70);
}
void GameScene::UpdateResult()
{
	if (input_->TriggerKey(DIK_C)) ChangeState(GameSceneState::RESULT);

	resultMenu_->Update();
	scoreDraw_->UpdateResult(GetStateElapsedTime());

	uiCollider_.BuildAABBs(false);

	if(GetStateElapsedTime() > 1.0f)
	{
		// 常に少しずつ追加して降り続ける
		confetti_.Emit(2);
		confetti_.Update(Timer::GetInstance()->GetDeltaTime());
	}

	if (GetStateElapsedTime() < 4.0f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((GetStateElapsedTime()) / 4.0f) * 13.0f);
	}
	else if (GetStateElapsedTime() > 4.5f)
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