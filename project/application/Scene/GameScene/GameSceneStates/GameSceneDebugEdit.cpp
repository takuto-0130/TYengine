#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"

void GameScene::InitDebugEdit()
{
}
void GameScene::UpdateDebugEdit()
{
	stageManager_->EditUpdate();
}
void GameScene::ExitDebugEdit()
{
}

void GameScene::SwitchEdit()
{
#ifdef _DEBUG
	ImGui::Begin("Play : Editor Switch");
	if (otherEditorSwitch_)
	{
		ImGui::Checkbox("Other Editor Switch", &otherEditorSwitch_);
		if (!otherEditorSwitch_) stateMachine_.ChangeState(GameSceneState::PLAY);
	}
	else
	{
		ImGui::Checkbox("Other Editor Switch", &otherEditorSwitch_);
		if (otherEditorSwitch_) stateMachine_.ChangeState(GameSceneState::DEBUG_EDIT);
	}
	ImGui::End();
#endif // _DEBUG
}