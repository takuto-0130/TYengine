#include "../GameScene.h"
#include "../../Transition/Fade/FadeTransition.h"
#include "../../Transition/TransitionManager.h"

void GameScene::InitFadeIn()
{
	// 1フレームだけカメラを動かす
	RailCameraMove();
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