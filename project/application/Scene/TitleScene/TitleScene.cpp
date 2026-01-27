#include "TitleScene.h"
#include "SceneManager.h"
#include "Object3dBasis.h"
#include "SpriteBasis.h"
#include "../Transition/Fade/FadeTransition.h"
#include "../Transition/Fade2/BlockFadeTransition.h"
#include "../Transition/TransitionManager.h"
#include "CubemapBasis.h"
#include "Timer.h"
#include "Audio/Audio.h"
#include "../../AppSystem/Audio/GameAudio.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	//========== ロード ==========//

	titleJM.Load("Title.json", true, &err);
#ifdef _DEBUG
	Logger::Log(err);
	Audio::GetInstance()->LoadWave("gameBGM");
#endif // _DEBUG


	//========== カメラ、入力取得 ==========//

	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();


	//========== テクスチャ初期化 ==========//

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Enter.png");
	spaceSpr_ = std::make_unique<Sprite>();
	spaceSpr_->Initialize("Resources/Texture/Enter.png");
	spaceSpr_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.Enter.AnchorPoint"));
	spaceSpr_->SetPosition(titleJM.Get<Vector2>("config.texture.Enter.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Title.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/Title.png");
	text_->SetPosition(titleJM.Get<Vector2>("config.texture.Title.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Operation.png");
	operation_ = std::make_unique<Sprite>();
	operation_->Initialize("Resources/Texture/Operation.png");
	operation_->SetPosition(titleJM.Get<Vector2>("config.texture.Operation.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.reticle.AnchorPoint"));


	//========== フィールド初期化 ==========//

	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/output_skybox.dds");

	ground_ = std::make_unique<Object3d>();
	ground_->Initialize();
	ground_->SetModel("titleground.obj");
	groundWT_.Initialize();
	groundWT_.TransferMatrix();
	rotateSpeed_ = titleJM.Get<float>("config.ground.rotateSpeed");


	//========== カメラ初期化 ==========//

	camera_->SetRotate(titleJM.Get<Vector3>("config.Camera.Rotate"));
	camera_->SetTranslate(titleJM.Get<Vector3>("config.Camera.Position"));


	//========== キャラクター初期化 ==========//

	player_ = std::make_unique<Player>();
	player_->SetCamera(camera_);
	player_->Init();
	player_->SetScreenOffset(titleJM.Get<Vector2>("config.Player.ScreenOffset"));

	enemyMgr_.Init(camera_);
}

void TitleScene::DebugJMApply()
{
	spaceSpr_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.Enter.AnchorPoint"));
	spaceSpr_->SetPosition(titleJM.Get<Vector2>("config.texture.Enter.Position"));
	text_->SetPosition(titleJM.Get<Vector2>("config.texture.Title.Position"));
	camera_->SetRotate(titleJM.Get<Vector3>("config.Camera.Rotate"));
	camera_->SetTranslate(titleJM.Get<Vector3>("config.Camera.Position"));
	operation_->SetPosition(titleJM.Get<Vector2>("config.texture.Operation.Position"));
	reticle_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.reticle.AnchorPoint"));
	rotateSpeed_ = titleJM.Get<float>("config.ground.rotateSpeed");
}

void TitleScene::Update() {
	Transition();
	enemyMgr_.SetCamera(camera_);
#ifdef _DEBUG

	audioAnalyzer_.Update();
	audioAnalyzer_.Draw();

	if (input_->TriggerKey(DIK_M))
	{
		int a = GameAudio::GetInstance()->Play("gameBGM", true, SoundCategory::BGM);
		GameAudio::GetInstance()->SetSoundVolume(a, 1.0f);
	}

	// ImGui で編集
	ImGui::Begin("JSON Editor");
	static jx::JsonImGuiEditor inspector(titleJM);
	inspector.Draw(titleJM.Root(), "Title.json");
	if (ImGui::Button("Save")) titleJM.Save();
	ImGui::End();

	DebugJMApply();

#else // Release

#endif // _DEBUG
	spaceSpr_->Update();
	text_->Update();
	operation_->Update();
	reticle_->Update();

	player_->Update();

	skybox_->Update();

	Vector3 pos = player_->GetWorldPosition();
	enemyMgr_.SetTargetPos(&pos);

	enemyMgr_.Update();

	groundWT_.rotation_.y -= rotateSpeed_ * Timer::GetInstance()->GetDeltaTime();
	groundWT_.TransferMatrix();

	Vector2 mouse = input_->GetMousePosition();
	reticle_->SetPosition(mouse);
}

void TitleScene::Draw() 
{
	CubemapBasis::GetInstance()->DrawBegin();
	skybox_->Draw();

	Object3dBasis::GetInstance()->BasisDrawSetting();

	ground_->Draw(groundWT_);

	enemyMgr_.Draw();

	player_->Draw();
}

void TitleScene::UIDraw()
{
	SpriteBasis::GetInstance()->BasisDrawSetting();
	spaceSpr_->Draw();
	text_->Draw();
	operation_->Draw();
	reticle_->Draw();
}

void TitleScene::Transition()
{
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN))
	{
		sceneManager_->ChangeScene("GAME");
	}
#else
	if (input_->TriggerKey(DIK_RETURN))
	{
		BlockFadeConfig cfg;
		auto transition = std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_IN, cfg);
		transition->SetOnFinishCallback([this]()
			{
				BlockFadeConfig cfg1;
				sceneManager_->ChangeScene("GAME");
				TransitionManager::GetInstance()->Enqueue(std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_OUT, cfg1));
			});
		TransitionManager::GetInstance()->Start(std::move(transition));
	}
#endif // _DEBUG
}
