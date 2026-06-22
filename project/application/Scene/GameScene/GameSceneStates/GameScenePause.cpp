#include "../GameScene.h"
#include "../Pause/Pause.h"

void GameSceneStatePause::Init(GameScene& owner)
{
	owner.pauseMenu_->Reset();
}

void GameSceneStatePause::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.pauseMenu_->Update();
	if (owner.input_->TriggerKey(DIK_ESCAPE)) 
	{
		owner.stateMachine_.ChangeState(GameSceneState::PLAY);
		owner.gameAudio_->Play("close", false, SoundCategory::UI);
	}
	if (owner.pauseMenu_->GetElements() == ButtonElements::RESUME) 
	{
		owner.stateMachine_.ChangeState(GameSceneState::PLAY);
		owner.gameAudio_->Play("close", false, SoundCategory::UI);
	}
	if (owner.pauseMenu_->GetElements() == ButtonElements::RETURN_TITLE)
	{
		owner.stateMachine_.ChangeState(GameSceneState::FADE_OUT);
		owner.gameAudio_->Play("enter", false, SoundCategory::UI);
	}
}

void GameSceneStatePause::Exit(GameScene& owner)
{
	(void)owner;
}