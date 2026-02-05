#include "../GameScene.h"
#include "PostEffectManager.h"
#include "GaussianEffect.h"
#include "../Result/Result.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "Timer.h"

using namespace TYEngine;
using namespace TYEngine::Core;
using namespace TYEngine::Graphics;
using namespace TYEngine::OffScreen;

void GameScene::InitResult()
{
	gameAudio_->Play("fanfare", false, SoundCategory::BGM);

	// ブラーエフェクトを有効化
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(13);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	
	// リザルトUIの初期化・開始
	resultMenu_->Start();
	scoreDraw_->SetResult();
	scoreDraw_->UpdateResult(0.0f);

	// 紙吹雪（Confetti）の衝突判定用にUIスプライトを登録
	std::vector<Graphics::Sprite*> setSpr;
	setSpr.insert(setSpr.end(), resultMenu_->GetSprite().begin(), resultMenu_->GetSprite().end());
	setSpr.insert(setSpr.end(), scoreDraw_->GetSprite().begin(), scoreDraw_->GetSprite().end());

	uiCollider_.SetSprites(setSpr);

	confetti_.Init(
		400,
		"Resources/Texture/white2x2.png",
		WindowsApp::kClientWidth,
		WindowsApp::kClientHeight
	);

	confetti_.SetUIColliders(uiCollider_.GetAABBs());

	// リザルト突入と同時に一気に出す
	confetti_.Burst(70);
}
void GameScene::UpdateResult()
{
	resultMenu_->Update();
	scoreDraw_->UpdateResult(GetStateElapsedTime());

	// UIとの衝突判定を更新
	uiCollider_.BuildAABBs(false);

	if(GetStateElapsedTime() > 1.0f)
	{
		// 常に少しずつ追加して降り続ける
		confetti_.Emit(2);
		confetti_.Update(Utility::Timer::GetInstance()->GetDeltaTime());
	}

	// 時間経過でブラー強度を変化、その後入力待ち
	if (GetStateElapsedTime() < 4.0f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((GetStateElapsedTime()) / 4.0f) * 13.0f);
	}
	else if (GetStateElapsedTime() > 4.5f)
	{
		if (input_->TriggerKey(DIK_SPACE)) 
		{
			gameAudio_->Play("enter", false, SoundCategory::UI);
			ChangeState(GameSceneState::FADE_OUT);
		}
	}
}
void GameScene::ExitResult()
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	resultMenu_->Reset();
}