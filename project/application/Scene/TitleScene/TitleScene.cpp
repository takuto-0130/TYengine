#include "TitleScene.h"
#include "SceneManager.h"
#include "Object3dBasis.h"
#include "SpriteBasis.h"
#include "../Transition/Fade/FadeTransition.h"
#include "../Transition/TransitionManager.h"
#include "CubemapBasis.h"
#include "Timer.h"
//#include "Sprite.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void TitleScene::Init() 
{ 
	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();
	loader_ = std::make_unique<BlenderLevelLoader>("Resources/JSON/");
	
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Enter.png");
	spaceSpr_ = std::make_unique<Sprite>();
	spaceSpr_->Initialize("Resources/Texture/Enter.png");
	spaceSpr_->SetAnchorPoint({ 0.5f,0.5f });
	spaceSpr_->SetPosition({ 1040,640 }); // 640,440


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Title.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/Title.png");
	text_->SetPosition({ -240,-200 }); // 40,0


	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/output_skybox.dds");

	camera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	camera_->SetTranslate({ -18.0f, 6.0f, -34.0f });

	player_ = std::make_unique<Player>();
	player_->SetCamera(camera_);
	player_->Init();
	player_->SetScreenOffset({ 0, -0.85f });

	enemyMgr_.Init(camera_);



	ground_ = std::make_unique<Object3d>();
	ground_->Initialize();
	ground_->SetModel("titleground.obj");
	groundWT_.Initialize();
	groundWT_.TransferMatrix();


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Operation.png");
	operation_ = std::make_unique<Sprite>();
	operation_->Initialize("Resources/Texture/Operation.png");
	operation_->SetPosition({ 0.0f, 180.0f });


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint({ 0.5f,0.5f });
}

void TitleScene::Update() {
	Transition();
#ifdef _DEBUG
	/*if (input_->TriggerKey(DIK_RETURN))
	{
		LoadLevel();
	}*/
	/*ImGui::Begin("");

	ImGui::End();*/
#endif // _DEBUG
	spaceSpr_->Update();
	text_->Update();
	operation_->Update();
	reticle_->Update();
	for (auto&& obj : objects_)
	{
		obj->Update();
	}
	player_->Update();
	skybox_->Update();

	Vector3 pos = player_->GetWorldPosition();
	enemyMgr_.SetTargetPos(&pos);

	enemyMgr_.Update();

	groundWT_.rotation_.y -= 0.1f * Timer::GetInstance()->GetDeltaTime();
	groundWT_.TransferMatrix();

	Vector2 mouse = input_->GetMousePosition();
	reticle_->SetPosition(mouse);
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

	ground_->Draw(groundWT_);
}

void TitleScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();
	spaceSpr_->Draw();
	text_->Draw();
	operation_->Draw();
	reticle_->Draw();
}

void TitleScene::LoadLevel()
{
	LevelData* levelData = loader_->Load("level.json");

	loader_->DataToObject(levelData, objects_);
}

void TitleScene::Transition()
{
	if (input_->TriggerKey(DIK_RETURN)) 
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
