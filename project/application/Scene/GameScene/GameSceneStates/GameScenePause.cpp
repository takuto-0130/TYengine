#include "../GameScene.h"
#include "../Pause/PauseUI.h"
#include "UIManager.h"

void GameSceneStatePause::Init(GameScene& owner)
{
	(void)owner;
	if (auto* pause = UIManager::GetInstance()->GetUI<PauseUI>("Pause"))
	{
		pause->Reset();
	}
}

void GameSceneStatePause::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	auto* uiMgr = UIManager::GetInstance();
	uiMgr->UpdateUI("Pause");

	if (owner.input_->TriggerKey(DIK_ESCAPE)) 
	{
		owner.stateMachine_.ChangeState(GameSceneState::PLAY);
		owner.gameAudio_->Play("close", false, SoundCategory::UI);
		return;
	}

	if (auto* pause = uiMgr->GetUI<PauseUI>("Pause"))
	{
		if (pause->GetElements() == ButtonElements::RESUME) 
		{
			owner.stateMachine_.ChangeState(GameSceneState::PLAY);
			owner.gameAudio_->Play("close", false, SoundCategory::UI);
		}
		if (pause->GetElements() == ButtonElements::RETURN_TITLE)
		{
			owner.stateMachine_.ChangeState(GameSceneState::FADE_OUT);
			owner.gameAudio_->Play("enter", false, SoundCategory::UI);
		}
	}
}

void GameSceneStatePause::Exit(GameScene& owner)
{
	(void)owner;
}