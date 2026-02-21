#include "../TitleScene.h"
#include "../../Transition/TransitionManager.h"

void TitleScene::InitFadeIn()
{}
void TitleScene::UpdateFadeIn()
{

	if (!TransitionManager::GetInstance()->IsBusy())
	{
		stateMachine_.ChangeState(TitleSceneState::READY);
	}

}
void TitleScene::ExitFadeIn()
{}