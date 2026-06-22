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

void GameSceneStateResult::Init(GameScene& owner)
{
	owner.gameAudio_->Play("fanfare", false, SoundCategory::BGM);

	// ブラーエフェクトを有効化
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", true);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetKernelSize(13);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	
	// リザルトUIの初期化・開始
	owner.resultMenu_->Start();
	owner.scoreDraw_->SetResult();
	owner.scoreDraw_->UpdateResult(0.0f);

	// 紙吹雪（Confetti）の衝突判定用にUIスプライトを登録
	std::vector<Graphics::Sprite*> setSpr;
	setSpr.insert(setSpr.end(), owner.resultMenu_->GetSprite().begin(), owner.resultMenu_->GetSprite().end());
	setSpr.insert(setSpr.end(), owner.scoreDraw_->GetSprite().begin(), owner.scoreDraw_->GetSprite().end());

	owner.uiCollider_.SetSprites(setSpr);

	owner.confetti_.Init(
		400,
		"Resources/Texture/white2x2.png",
		WindowsApp::kClientWidth,
		WindowsApp::kClientHeight
	);

	owner.confetti_.SetUIColliders(owner.uiCollider_.GetAABBs());

	// リザルト突入と同時に一気に出す
	owner.confetti_.Burst(70);
}

void GameSceneStateResult::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.resultMenu_->Update();
	owner.scoreDraw_->UpdateResult(owner.stateMachine_.GetStateElapsedTime());

	// UIとの衝突判定を更新
	owner.uiCollider_.BuildAABBs(false);

	if(owner.stateMachine_.GetStateElapsedTime() > 1.0f)
	{
		// 常に少しずつ追加して降り続ける
		owner.confetti_.Emit(2);
		owner.confetti_.Update(Utility::Timer::GetInstance()->GetDeltaTime());
	}

	// 時間経過でブラー強度を変化、その後入力待ち
	if (owner.stateMachine_.GetStateElapsedTime() < 4.0f)
	{
		PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(((owner.stateMachine_.GetStateElapsedTime()) / 4.0f) * 13.0f);
	}
	else if (owner.stateMachine_.GetStateElapsedTime() > 4.5f)
	{
		if (owner.input_->TriggerKey(DIK_SPACE)) 
		{
			owner.gameAudio_->Play("enter", false, SoundCategory::UI);
			owner.stateMachine_.ChangeState(GameSceneState::FADE_OUT);
		}
	}
}

void GameSceneStateResult::Exit(GameScene& owner)
{
	PostEffectManager::GetInstance()->SetEffectEnabled("Gaussian", false);
	PostEffectManager::GetInstance()->GetEffect<GaussianEffect>("Gaussian")->SetSigma(0.0f);
	owner.resultMenu_->Reset();
}