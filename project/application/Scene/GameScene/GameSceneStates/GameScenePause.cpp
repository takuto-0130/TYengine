#include "../GameScene.h"
#include "../Pause/Pause.h"

void GameScene::InitPause()
{
}
void GameScene::UpdatePause()
{
	pauseMenu_->Update();
	if (input_->TriggerKey(DIK_ESCAPE)) ChangeState(GameSceneState::PLAY);


	if (input_->TriggerKey(DIK_T)) ChangeState(GameSceneState::FADE_OUT);
}
void GameScene::ExitPause()
{
}