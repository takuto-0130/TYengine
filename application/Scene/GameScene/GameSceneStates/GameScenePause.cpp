#include "../GameScene.h"
#include "../Pause/Pause.h"

void GameScene::InitPause()
{
}
void GameScene::UpdatePause()
{
	pauseMenu_->Update();
	if (input_->TriggerKey(DIK_ESCAPE)) ChangeState(GameSceneState::PLAY);
}
void GameScene::ExitPause()
{
}