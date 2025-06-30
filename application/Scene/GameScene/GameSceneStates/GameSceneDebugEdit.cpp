#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"

void GameScene::InitDebugEdit()
{
}
void GameScene::UpdateDebugEdit()
{
	StageEdit();
	enemyManager_->UpdateEditorEnemies();
	railManager_->UpdateEdit();
}
void GameScene::ExitDebugEdit()
{
}