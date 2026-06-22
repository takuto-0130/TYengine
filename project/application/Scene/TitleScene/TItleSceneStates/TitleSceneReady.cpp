#include "../TitleScene.h"

void TitleSceneStateReady::Init(TitleScene& owner)
{
	(void)owner;
}

void TitleSceneStateReady::Update(TitleScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.stateMachine_.ChangeState(TitleSceneState::PLAY);
}

void TitleSceneStateReady::Exit(TitleScene& owner)
{
	(void)owner;
}