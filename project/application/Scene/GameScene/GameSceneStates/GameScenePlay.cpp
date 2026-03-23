#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#include "../PlayUI/PlayUI.h"
#include "../../../ScoreUI/ScoreUI.h"

using namespace TYEngine::CameraSystem;

void GameScene::InitPlay()
{
}
void GameScene::UpdatePlay()
{
	// コンボUIの更新
	ComboUIUpdate();

	// ステージ（レール進行・敵の発生など）更新
	stageManager_->Update();

	// レール終端到達でリザルトへ
	if (stageManager_->EndRail()) stateMachine_.ChangeState(GameSceneState::RESULT);

	PlayUIUpdate();

	// ポーズ画面へ
	if (input_->TriggerKey(DIK_ESCAPE)) 
	{
		gameAudio_->Play("open", false, SoundCategory::UI);
		stateMachine_.ChangeState(GameSceneState::PAUSE);
	}

	// プレイヤー死亡時は死亡演出へ
	if (stageManager_->GetPlayer()->GetStateMachine().GetCurrentState() == PlayerState::DEAD)
	{
		stateMachine_.ChangeState(GameSceneState::DEAD);
	}
}
void GameScene::ExitPlay()
{
}

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