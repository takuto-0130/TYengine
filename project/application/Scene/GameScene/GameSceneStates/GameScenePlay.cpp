#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#include "../PlayUI/PlayUI.h"
#include "../../../ScoreUI/ScoreUI.h"

void GameScene::InitPlay()
{
}
void GameScene::UpdatePlay()
{
	ComboUIUpdate();

	stageManager_->Update();

	if (stageManager_->EndRail()) ChangeState(GameSceneState::RESULT);

	PlayUIUpdate();

	if (input_->TriggerKey(DIK_ESCAPE)) ChangeState(GameSceneState::PAUSE);

	if (input_->TriggerKey(DIK_T)) ChangeState(GameSceneState::FADE_OUT);

	if (stageManager_->GetPlayer()->IsDead())
	{
		CameraShake::ShakeParams params;
		params.duration = 0.6f;
		params.amplitude = 0.15f;
		params.frequency = 20.0f;

		camera_->StartShake(params);
		ChangeState(GameSceneState::RETRY);
	}

	/*if(isReady_ && GetStateElapsedTime() >= 0.3f) 
	{
		isReady_ = false;
		ChangeState(GameSceneState::FADE_IN);
	}*/
}
void GameScene::ExitPlay()
{
}

void GameScene::PlayUIUpdate()
{
	scoreDraw_->Update();
	playUI_->Update();
}

void GameScene::ComboUIUpdate()
{
	ComboManager* combo = stageManager_->GetComboManager();
	playUI_->SetComboTime(combo->GetStartComboTime());
	playUI_->SetComboTimer(combo->GetCurrentComboTimer());
	playUI_->SetComboNum(combo->GetComboCount());
	playUI_->ComboTexUpdate();
	if (combo->GetCurrentComboTimer() == combo->GetStartComboTime())
	{
		scoreDraw_->SetScore(stageManager_->GetScoreManager()->GetScore());
	}
}