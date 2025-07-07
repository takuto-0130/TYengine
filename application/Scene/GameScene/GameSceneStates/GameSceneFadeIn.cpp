#include "../GameScene.h"
#include "../../Transition/Fade/FadeTransition.h"
#include "../../Transition/TransitionManager.h"
#include "../../../Object/Rail/RailManager.h"

void GameScene::InitFadeIn()
{
}
void GameScene::UpdateFadeIn()
{
	if (!TransitionManager::GetInstance()->IsBusy())
	{
		ChangeState(GameSceneState::PLAY);
	}
}
void GameScene::ExitFadeIn()
{
}