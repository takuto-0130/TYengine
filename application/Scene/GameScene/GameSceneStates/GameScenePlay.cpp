#include "../GameScene.h"

void GameScene::InitPlay()
{
}
void GameScene::UpdatePlay()
{
#ifdef _DEBUG
	RailCameraDebug();
#else
	RailCameraMove();
#endif // !_DEBUG

	enemyManager_->Update();

	AttackUpdate();

	PlayUIUpdate();

	if (input_->TriggerKey(DIK_ESCAPE)) ChangeState(GameSceneState::PAUSE);
}
void GameScene::ExitPlay()
{
}