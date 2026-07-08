#include "../GameScene.h"
#include "../../../Object/Rail/RailManager.h"
#ifdef _DEBUG
#include "imgui.h"
#endif

#include "Utils/Json/JsonManager.h"
using namespace TYEngine::Utility;

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

void GameScene::DebugDraw()
{
#ifdef _DEBUG
	ImGui::Begin("GameScene State Debug");
	stateMachine_.DebugImGui("GameScene");
	ImGui::End();

	// ImGui で編集
	// UI
	ImGui::Begin("JSON Editor");
	static JsonImGuiEditor inspectorUI(*gameUIJM_);
	inspectorUI.Draw(gameUIJM_->Root(), "GameUI.json");
	if (ImGui::Button("SaveUI")) gameUIJM_->Save();
	// パラメータ
	static JsonImGuiEditor inspectorParam(*paramJM_);
	inspectorParam.Draw(paramJM_->Root(), "Param.json");
	if (ImGui::Button("SaveParam")) paramJM_->Save();
	// コンフィグ
	static JsonImGuiEditor inspectorConfig(*configJM_);
	inspectorConfig.Draw(configJM_->Root(), "Config.json");
	if (ImGui::Button("SaveConfig")) configJM_->Save();
	ImGui::End();
#endif // _DEBUG
}
