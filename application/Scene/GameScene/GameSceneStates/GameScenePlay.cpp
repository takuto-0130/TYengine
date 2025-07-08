#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#include "../PlayUI/PlayUI.h"
#include "../../../Score/score.h"

void GameScene::InitPlay()
{
}
void GameScene::UpdatePlay()
{
	stageManager_->Update();

	if (stageManager_->EndRail()) ChangeState(GameSceneState::RESULT);

	AttackUpdate();

	PlayUIUpdate();

	if (input_->TriggerKey(DIK_ESCAPE)) ChangeState(GameSceneState::PAUSE);
}
void GameScene::ExitPlay()
{
}

void GameScene::PlayUIUpdate()
{
	scoreDraw_->Update();
	playUI_->Update();
}

void GameScene::AttackUpdate()
{
	if (input_->PushKey(DIK_SPACE)) Collision();

	if (comboTimer_ > 0)
	{
		comboTimer_ -= 1.0f / 60.0f;
		if (comboTimer_ < 0)
		{
			comboTimer_ = 0;
		}
	}
	playUI_->SetComboTime(kComboTime_);
	playUI_->SetComboTimer(comboTimer_);
	playUI_->ComboTexUpdate();
}