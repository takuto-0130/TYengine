#include "../TitleScene.h"
#include "Timer.h"

void TitleScene::InitReady()
{
}
void TitleScene::UpdateReady()
{
	stateMachine_.ChangeState(TitleSceneState::PLAY);
}
void TitleScene::ExitReady()
{
}