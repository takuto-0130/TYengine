#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"

void GameScene::InitPlay()
{
}
void GameScene::UpdatePlay()
{
	enemyManager_->Update();

	railManager_->Update();

	if (railManager_->RailTrigger()) enemyManager_->TriggerNextEnemyGroup();

	AttackUpdate();

	PlayUIUpdate();

	if (input_->TriggerKey(DIK_ESCAPE)) ChangeState(GameSceneState::PAUSE);
}
void GameScene::ExitPlay()
{
}