#include "../GameScene.h"
#include "../../Transition/Fade/FadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "../../../Object/Rail/RailManager.h"

void GameScene::InitFadeIn()
{
}
void GameScene::UpdateFadeIn()
{
	if (readyCount_ < 2)
	{
		++readyCount_;
		ComboUIUpdate();
		PlayUIUpdate();
		stageManager_->GetPlayer()->Update();
	}
	if(readyCount_ == 2)
	{
		++readyCount_;
		StartCamera();
	}

	if (!TransitionManager::GetInstance()->IsBusy())
	{
		ChangeState(GameSceneState::READY);
	}

}
void GameScene::ExitFadeIn()
{
}