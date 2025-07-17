#include "../GameScene.h"
#include "../Result/Result.h"
#include "../../../ScoreUI/ScoreUI.h"

void GameScene::InitResult()
{
	scoreDraw_->SetResult();
	scoreDraw_->Update();
}
void GameScene::UpdateResult()
{
	resultMenu_->Update();
	if (input_->TriggerKey(DIK_SPACE)) ChangeState(GameSceneState::FADE_OUT);
}
void GameScene::ExitResult()
{
}