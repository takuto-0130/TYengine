#include "../GameScene.h"
#include "../Pause/Pause.h"

void GameScene::InitPause()
{
	pauseMenu_->Reset();
}
void GameScene::UpdatePause()
{
	pauseMenu_->Update();
	if (input_->TriggerKey(DIK_ESCAPE)) 
	{
		stateMachine_.ChangeState(GameSceneState::PLAY);
		gameAudio_->Play("close", false, SoundCategory::UI);
	}
	if (pauseMenu_->GetElements() == ButtonElements::RESUME) 
	{
		stateMachine_.ChangeState(GameSceneState::PLAY);
		gameAudio_->Play("close", false, SoundCategory::UI);
	}
	if (pauseMenu_->GetElements() == ButtonElements::RETURN_TITLE)
	{
		stateMachine_.ChangeState(GameSceneState::FADE_OUT);
		gameAudio_->Play("enter", false, SoundCategory::UI);
	}
}
void GameScene::ExitPause()
{
}