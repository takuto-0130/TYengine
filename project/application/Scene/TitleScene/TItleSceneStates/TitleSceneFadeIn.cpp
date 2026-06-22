#include "../TitleScene.h"
#include "../../Transition/TransitionManager.h"

void TitleSceneStateFadeIn::Init(TitleScene& owner)
{
	(void)owner;
}

void TitleSceneStateFadeIn::Update(TitleScene& owner, float deltaTime)
{
	(void)deltaTime;
	if (!TransitionManager::GetInstance()->IsBusy())
	{
		owner.stateMachine_.ChangeState(TitleSceneState::READY);
	}
}

void TitleSceneStateFadeIn::Exit(TitleScene& owner)
{
	(void)owner;
}