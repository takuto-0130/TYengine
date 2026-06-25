#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#include "../PlayUI/PlayUI.h"
#include "../../../ScoreUI/ScoreUI.h"
#include "UIManager.h"

using namespace TYEngine::CameraSystem;

void GameSceneStatePlay::Init(GameScene& owner)
{
	owner.gameAudio_->Resume(owner.BGMHandle_);
}

void GameSceneStatePlay::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	// コンボUIの更新
	owner.ComboUIUpdate();

	// ステージ（レール進行・敵の発生など）更新
	owner.stageManager_->Update();

	// レール終端到達でリザルトへ
	if (owner.stageManager_->EndRail()) owner.stateMachine_.ChangeState(GameSceneState::RESULT);

	owner.PlayUIUpdate();

	// ポーズ画面へ
	if (owner.input_->TriggerKey(DIK_ESCAPE)) 
	{
		owner.gameAudio_->Play("open", false, SoundCategory::UI);
		owner.stateMachine_.ChangeState(GameSceneState::PAUSE);
	}

	// プレイヤー死亡時は死亡演出へ
	if (owner.stageManager_->GetPlayer()->GetStateMachine().GetCurrentState() == PlayerState::DEAD)
	{
		owner.stateMachine_.ChangeState(GameSceneState::DEAD);
	}
}

void GameSceneStatePlay::Exit(GameScene& owner)
{
	(void)owner;
}

// GameSceneのメンバ関数
void GameScene::PlayUIUpdate()
{
	auto* uiMgr = UIManager::GetInstance();
	uiMgr->UpdateUI("Score");

	auto* play = uiMgr->GetUI<PlayUI>("Play");
	if (play)
	{
		// バレルロール中以外はジャスト回避判定などをUIに反映
		if(stageManager_->GetPlayer()->GetStateMachine().GetCurrentState() != PlayerState::BARREL_ROLL)
		{
			play->SetJust(stageManager_->GetPlayer()->IsJust());
			stageManager_->GetPlayer()->OffJust();
			play->SetShiftPos(stageManager_->GetPlayer()->GetScreenOffset());
		}
		else
		{
			play->SetJust(false);
		}
		play->Update();
	}
}

void GameScene::ComboUIUpdate()
{
	auto* uiMgr = UIManager::GetInstance();
	auto* play = uiMgr->GetUI<PlayUI>("Play");
	auto* score = uiMgr->GetUI<ScoreUI>("Score");

	HitStreakManager* combo = stageManager_->GetComboManager();
	if (play)
	{
		play->SetComboTime(combo->GetStartComboTime());
		play->SetComboTimer(combo->GetCurrentComboTimer());
		play->SetComboNum(combo->GetComboCount());
		play->ComboTexUpdate();
	}
	if (score)
	{
		score->SetScore(stageManager_->GetScoreManager()->GetScore());
	}
}