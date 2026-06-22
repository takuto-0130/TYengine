#include "../GameScene.h"

using namespace TYEngine::CameraSystem;

void GameSceneStateDead::Init(GameScene& owner)
{
	CameraShake::ShakeParams params;
	params.duration = 1.6f;
	params.amplitude = 0.15f;
	params.frequency = 20.0f;

	owner.camera_->StartShake(params);
}

void GameSceneStateDead::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.stageManager_->GetPlayer()->Update();
	// 演出終了後、リトライ誘導画面へ
	if (owner.stageManager_->GetPlayer()->IsDead())
	{
		owner.stateMachine_.ChangeState(GameSceneState::RETRY);
	}
}

void GameSceneStateDead::Exit(GameScene& owner)
{
	(void)owner;
}