#include "TitleScene.h"
#include "SceneManager.h"
#include "Object3dBasis.h"
#include "SpriteBasis.h"
#include "../Transition/Fade/FadeTransition.h"
#include "../Transition/Fade2/BlockFadeTransition.h"
#include "../Transition/TransitionManager.h"
#include "CubemapBasis.h"
#include "Timer.h"
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
	Load();

	beatAnalyzer_.Init("gameBGM", gameAudio_->CategoryToString(SoundCategory::BGM));

	bgmHandle_ = gameAudio_->Play("gameBGM", false, SoundCategory::BGM);
	gameAudio_->SetSoundVolume(bgmHandle_, 1.0f);


	//========== カメラ、入力取得 ==========//

	input_ = Framework::Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();


	//========== テクスチャ初期化 ==========//

	enterSpr_ = std::make_unique<Sprite>();
	enterSpr_->Initialize("Resources/Texture/Enter.png");
	enterSpr_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.Enter.AnchorPoint"));
	enterSpr_->SetPosition(titleJM.Get<Vector2>("config.texture.Enter.Position"));

	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/Title.png");
	text_->SetPosition(titleJM.Get<Vector2>("config.texture.Title.Position"));

	operation_ = std::make_unique<Sprite>();
	operation_->Initialize("Resources/Texture/Operation.png");
	operation_->SetPosition(titleJM.Get<Vector2>("config.texture.Operation.Position"));

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
	player_->SetBGMHandle(bgmHandle_);

	// タイトル画面の敵マネージャ
	enemyMgr_.Init(camera_);
	enemyMgr_.SetBeatAnalyzer(&beatAnalyzer_);

	player_->SetEnemyManager(&enemyMgr_);
}

void TitleScene::Update() {
	// シーン遷移更新
	Transition();
	beatAnalyzer_.Update();
	audioAnalyzer_.Update();

	// デバッグ用更新処理
	DebugUpdate();

	// UI更新
	(enterSpr_->IsMouseHover()) ? enterSpr_->SetScale({ 1.2f,1.2f }) : enterSpr_->SetScale({ 1.0f,1.0f });
	enterSpr_->Update();
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
	enterSpr_->Draw();
	text_->Draw();
	operation_->Draw();
	player_->DrawUI();
	reticle_->Draw();
}

void TitleScene::Transition()
{
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN) || enterSpr_->IsMouseClicked(0))
	{
		sceneManager_->ChangeScene("GAME");
		gameAudio_->Stop(bgmHandle_);
	}
#else
	if (input_->TriggerKey(DIK_RETURN) || enterSpr_->IsMouseClicked(0))
	{
		// 決定音再生
		gameAudio_->Play("enter", false, SoundCategory::UI);
		
		// ブロックフェード演出を開始してゲームシーンへ遷移
		BlockFadeConfig cfg;
		auto transition = std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_IN, cfg);
		transition->SetOnFinishCallback([this]()
			{
				BlockFadeConfig cfg1;
				sceneManager_->ChangeScene("GAME");
				TransitionManager::GetInstance()->Enqueue(std::make_unique<BlockFadeTransition>(BlockFadeTransition::Type::FADE_OUT, cfg1));
				gameAudio_->Stop(bgmHandle_);
			});
		TransitionManager::GetInstance()->Start(std::move(transition));
	}
#endif // _DEBUG
}
