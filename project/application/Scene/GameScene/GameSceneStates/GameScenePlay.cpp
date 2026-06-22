#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#include "../PlayUI/PlayUI.h"
#include "../../../ScoreUI/ScoreUI.h"

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
	scoreDraw_->Update();

	// バレルロール中以外はジャスト回避判定などをUIに反映
	if(stageManager_->GetPlayer()->GetStateMachine().GetCurrentState() != PlayerState::BARREL_ROLL)
	{
		playUI_->SetJust(stageManager_->GetPlayer()->IsJust());
		stageManager_->GetPlayer()->OffJust();
		playUI_->SetShiftPos(stageManager_->GetPlayer()->GetScreenOffset());
	}
	else
	{
		playUI_->SetJust(false);
	}

	playUI_->Update();
}

void GameScene::ComboUIUpdate()
{
	ComboManager* combo = stageManager_->GetComboManager();
	playUI_->SetComboTime(combo->GetStartComboTime());
	playUI_->SetComboTimer(combo->GetCurrentComboTimer());
	playUI_->SetComboNum(combo->GetComboCount());
	playUI_->ComboTexUpdate();
	scoreDraw_->SetScore(stageManager_->GetScoreManager()->GetScore());
	if (stageManager_->GetPlayer())
	{
		playUI_->SetHPNum(stageManager_->GetPlayer()->GetHP());
	}
	else
	{
		playUI_->SetHPNum(0);
	}
}