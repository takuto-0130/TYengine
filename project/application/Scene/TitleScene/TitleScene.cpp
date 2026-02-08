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

using namespace TYEngine::Utility;
using namespace TYEngine::Debugger;
using namespace TYEngine::AudioSystem;
using namespace TYEngine::Graphics;
using namespace TYEngine;


TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	//========== ロード ==========//

	// タイトル用JSONデータのロード
	titleJM.Load("Title.json", true, &err);
#ifdef _DEBUG
	Log(err);
	Audio::GetInstance()->LoadWave("gameBGM");
#endif // _DEBUG

	// 必要なサウンドリソースの読み込み
	GameAudio::GetInstance()->LoadSound("open");
	GameAudio::GetInstance()->LoadSound("close");
	GameAudio::GetInstance()->LoadSound("slide");
	GameAudio::GetInstance()->LoadSound("enter");
	GameAudio::GetInstance()->LoadSound("fanfare");
	GameAudio::GetInstance()->LoadSound("gameBGM");
	GameAudio::GetInstance()->LoadSound("gekiha");
	GameAudio::GetInstance()->LoadSound("damageE");
	GameAudio::GetInstance()->LoadSound("damageP");
	GameAudio::GetInstance()->LoadSound("roll");
	GameAudio::GetInstance()->LoadSound("attack");
	GameAudio::GetInstance()->LoadSound("418");

	beatAnalyzer_.Init("418", "BGM");


	//========== カメラ、入力取得 ==========//

	input_ = Framework::Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();


	//========== テクスチャ初期化 ==========//

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Enter.png");
	spaceSpr_ = std::make_unique<Graphics::Sprite>();
	spaceSpr_->Initialize("Resources/Texture/Enter.png");
	spaceSpr_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.Enter.AnchorPoint"));
	spaceSpr_->SetPosition(titleJM.Get<Vector2>("config.texture.Enter.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Title.png");
	text_ = std::make_unique<Graphics::Sprite>();
	text_->Initialize("Resources/Texture/Title.png");
	text_->SetPosition(titleJM.Get<Vector2>("config.texture.Title.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Operation.png");
	operation_ = std::make_unique<Graphics::Sprite>();
	operation_->Initialize("Resources/Texture/Operation.png");
	operation_->SetPosition(titleJM.Get<Vector2>("config.texture.Operation.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Graphics::Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.reticle.AnchorPoint"));


	//========== フィールド初期化 ==========//

	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/output_skybox.dds");

	ground_ = std::make_unique<Object3d>();
	ground_->Initialize();
	ground_->SetModel("titleground.obj");
	groundWT_.Initialize();
	groundWT_.Update();
	rotateSpeed_ = titleJM.Get<float>("config.ground.rotateSpeed");


	//========== カメラ初期化 ==========//

	// JSONから設定された初期位置・角度を反映
	camera_->SetRotate(titleJM.Get<Vector3>("config.Camera.Rotate"));
	camera_->SetTranslate(titleJM.Get<Vector3>("config.Camera.Position"));


	//========== キャラクター初期化 ==========//

	// タイトル画面の演出用プレイヤー
	player_ = std::make_unique<Player>();
	player_->SetCamera(camera_);
	player_->Init();
	player_->SetScreenOffset(titleJM.Get<Vector2>("config.Player.ScreenOffset"));

	// タイトル画面の演出用敵マネージャ
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
	// シーン遷移更新
	Transition();
	enemyMgr_.SetCamera(camera_);
#ifdef _DEBUG

	audioAnalyzer_.Update();
	audioAnalyzer_.Draw();
	beatAnalyzer_.Update();
	beatAnalyzer_.Draw();
	static int bgmH = -1;

	if (input_->TriggerKey(DIK_M))
	{
		bgmH = GameAudio::GetInstance()->Play("418", false, SoundCategory::BGM);
		GameAudio::GetInstance()->SetSoundVolume(bgmH, 1.0f);
	}
	if (input_->TriggerKey(DIK_P))
	{
		Audio::GetInstance()->Pause(bgmH);
	}
	if (input_->TriggerKey(DIK_O))
	{
		Audio::GetInstance()->ReStart(bgmH);
	}

	// ImGui で編集
	ImGui::Begin("JSON Editor");
	static TYEngine::Utility::JsonImGuiEditor inspector(titleJM);
	inspector.Draw(titleJM.Root(), "Title.json");
	if (ImGui::Button("Save")) titleJM.Save();
	ImGui::End();

	DebugJMApply();

	static float timer = 0.0f;
	if (beatAnalyzer_.GetBeat())
	{
		timer = 0.0f;
		player_->SetScale({ 0.4f,0.4f,0.4f });
	}
	else
	{
		timer += Timer::GetInstance()->GetDeltaTime();
		player_->SetScale(Lerp(Vector3{ 0.4f,0.4f,0.4f } , Vector3{ 0.2f,0.2f,0.2f }, timer));
	}

#else // Release

#endif // _DEBUG
	// UI更新
	spaceSpr_->Update();
	text_->Update();
	operation_->Update();
	reticle_->Update();

	// 演出用プレイヤー更新
	player_->Update();

	skybox_->Update();

	// エイム用照準位置の更新
	Vector3 pos = player_->GetWorldPosition();
	enemyMgr_.SetTargetPos(&pos);

	// 敵管理更新
	enemyMgr_.Update();
	
	// 地面の回転演出
	Vector3 rotate = groundWT_.GetRotate();
	rotate.y -= rotateSpeed_ * Timer::GetInstance()->GetDeltaTime();
	groundWT_.SetRotate(rotate);
	groundWT_.Update();

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
		// 決定音再生
		GameAudio::GetInstance()->Play("enter", false, SoundCategory::UI);
		
		// ブロックフェード演出を開始してゲームシーンへ遷移
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
