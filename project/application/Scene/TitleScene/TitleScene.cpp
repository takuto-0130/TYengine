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
	spaceSpr_->SetPosition({ 640,360 });


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleText.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/TitleText.png");


	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/output_skybox.dds");



	emitter.transform.scale = { 0.05f,1.0f,1.0f };
	emitter.transform.rotate = { 0,0,0 };
	emitter.transform.translate = { 0,1.3f,0 };
	emitter.count = 5;
	emitter.frequency = 1.5f;

	emitterRing.transform.scale = { 0.5f,0.5f,0.5f };
	emitterRing.transform.rotate = { 0,0,0 };
	emitterRing.transform.translate = { 0,0,0 };
	emitterRing.count = 1;
	emitterRing.frequency = 1.5f;
}

void TitleScene::Update() {
	Transition();
#ifdef _DEBUG
	/*if (input_->TriggerKey(DIK_RETURN))
	{
		LoadLevel();
		ParticleManager::GetInstance()->SetEmitter(0, emitter);
		ParticleManager::GetInstance()->TriggerEmit(0, true);
	}*/
#endif // _DEBUG
	spaceSpr_->Update();
	text_->Update();
	for (auto&& obj : objects_)
	{
		obj->Update();
	}
	skybox_->Update();
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
