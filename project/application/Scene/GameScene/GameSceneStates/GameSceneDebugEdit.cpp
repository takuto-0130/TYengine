#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#ifdef _DEBUG
#include "imgui.h"
#endif

void GameSceneStateDebugEdit::Init(GameScene& owner)
{
	(void)owner;
}

void GameSceneStateDebugEdit::Update(GameScene& owner, float deltaTime)
{
	(void)deltaTime;
	owner.stageManager_->EditUpdate();
}

void GameSceneStateDebugEdit::Exit(GameScene& owner)
{
	(void)owner;
}

// GameSceneのメンバ関数
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