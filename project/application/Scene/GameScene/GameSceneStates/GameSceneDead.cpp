#include "../GameScene.h"

using namespace TYEngine::CameraSystem;

void GameScene::InitDead()
{
	CameraShake::ShakeParams params;
	params.duration = 1.6f;
	params.amplitude = 0.15f;
	params.frequency = 20.0f;

	camera_->StartShake(params);
}
void GameScene::UpdateDead()
{
	stageManager_->GetPlayer()->Update();
	// 演出終了後、リトライ誘導画面へ
	if (stageManager_->GetPlayer()->IsDead())
	{
		stateMachine_.ChangeState(GameSceneState::RETRY);
	}
}
void GameScene::ExitDead()
{
}