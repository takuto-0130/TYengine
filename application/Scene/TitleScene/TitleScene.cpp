#include "TitleScene.h"
#include "SceneManager.h"
#include "ParticleManager.h"
#include "Object3dBasis.h"
#include "imgui.h"

void TitleScene::Init() 
{ 
	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();
	loader_ = std::make_unique<BlenderLevelLoader>("Resources/JSON/");
	
	
}

void TitleScene::Update() {
	if (input_->TriggerKey(DIK_RETURN))
	{
		LoadLevel();
	}
	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");
	}
#ifdef _DEBUG
	ImGui::Begin("TITLE");
	ImGui::Text("Space to GameScene");
	ImGui::End();
#endif // _DEBUG

	for (auto&& obj : objects_)
	{
		obj->Update();
	}
}

void TitleScene::Draw() 
{
	Object3dBasis::GetInstance()->BasisDrawSetting();
	for (auto& obj : objects_)
	{
		obj->Draw();
	}
}

void TitleScene::LoadLevel()
{
	LevelData* levelData = loader_->Load("level.json");

	loader_->DataToObject(levelData, objects_);
}
