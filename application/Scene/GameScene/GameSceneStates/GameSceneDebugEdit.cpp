#include "../GameScene.h"

void GameScene::InitDebugEdit()
{
}
void GameScene::UpdateDebugEdit()
{
	RailCameraDebug();
	StageEdit();
	enemyManager_->UpdateEditorEnemies();
}
void GameScene::ExitDebugEdit()
{
}