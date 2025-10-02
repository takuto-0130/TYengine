#include "TitleScene.h"
#include "SceneManager.h"
#include "Object3dBasis.h"
#include "SpriteBasis.h"
#include "../Transition/Fade/FadeTransition.h"
#include "../Transition/TransitionManager.h"
#include "CubemapBasis.h"
//#include "Sprite.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void TitleScene::Init() 
{ 
	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();
	loader_ = std::make_unique<BlenderLevelLoader>("Resources/JSON/");
	
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleSpace.png");
	spaceSpr_ = std::make_unique<Sprite>();
	spaceSpr_->Initialize("Resources/Texture/TitleSpace.png");
	spaceSpr_->SetAnchorPoint({ 0.5f,0.5f });
	spaceSpr_->SetPosition({ 640,440 });


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Title.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/Title.png");
	text_->SetPosition({ 40,0 });


	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/output_skybox.dds");


	player_ = std::make_unique<Player>();
	player_->SetCamera(camera_);
	player_->Init();
	player_->SetScreenOffset({ 0, -0.85f });

	enemyMgr_.Init(camera_);
}

void TitleScene::Update() {
	Transition();
#ifdef _DEBUG
	/*if (input_->TriggerKey(DIK_RETURN))
	{
		LoadLevel();
	}*/
#endif // _DEBUG
	spaceSpr_->Update();
	text_->Update();
	for (auto&& obj : objects_)
	{
		obj->Update();
	}
	player_->Update();
	skybox_->Update();

	Vector3 pos = player_->GetWorldPosition();
	enemyMgr_.SetTargetPos(&pos);

	enemyMgr_.Update();

	if (input_->TriggerKey(DIK_RETURN))
	{
		enemyMgr_.Pop();
	}
}

void TitleScene::Draw() 
{
	CubemapBasis::GetInstance()->DrawBegin();
	skybox_->Draw();

	Object3dBasis::GetInstance()->BasisDrawSetting();
	for (auto& obj : objects_)
	{
		obj->Draw();
	}
	player_->Draw();

	enemyMgr_.Draw();
}

void TitleScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();
	spaceSpr_->Draw();
	text_->Draw();
}

void TitleScene::LoadLevel()
{
	LevelData* levelData = loader_->Load("level.json");

	loader_->DataToObject(levelData, objects_);
}

void TitleScene::Transition()
{
	if (input_->TriggerKey(DIK_SPACE)) 
	{
		auto transition = std::make_unique<FadeTransition>(FadeTransition::Type::FADE_OUT, 1.0f);
		transition->SetOnFinishCallback([this]() 
			{
			sceneManager_->ChangeScene("GAME");
			TransitionManager::GetInstance()->Enqueue(std::make_unique<FadeTransition>(FadeTransition::Type::FADE_IN, 1.0f));
			});
		TransitionManager::GetInstance()->Start(std::move(transition));
	}
}
