#include "../GameScene.h"
#include "SceneManager.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../RetryUI/RetryUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "../../Transition/Fade2/BlockFadeTransition.h"
#include "../../Transition/TransitionManager.h"


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
		//if (input_->TriggerKey(DIK_RETURN))
		//{
		//	gameAudio_->Play("enter", false, SoundCategory::UI);
		//	// ブロックフェード演出を開始してゲームシーンへ遷移
		//	BlockFadeConfig cfg;
		//	auto transition = std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_IN, cfg);
		//	transition->SetOnFinishCallback([this]()
		//		{
		//			BlockFadeConfig cfg1;
		//			sceneManager_->ChangeScene("GAME");
		//			TransitionManager::GetInstance()->Enqueue(std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_OUT, cfg1));
		//			gameAudio_->Stop(BGMHandle_);
		//		});
		//	TransitionManager::GetInstance()->Start(std::move(transition));
		//}
	}
}

void GameScene::ExitRetry()
{
	retryDraw_->Reset();
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
}