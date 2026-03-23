#include "TitleScene.h"
#include "Object3dBasis.h"
#include "SpriteBasis.h"
#include "CubemapBasis.h"
#include "Timer.h"
#include "../../AppSystem/Audio/GameAudio.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

using namespace TYEngine;
using namespace Utility;
using namespace Debugger;
using namespace AudioSystem;
using namespace Graphics;

#define TITLE_SCENE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(TitleScene, stateEnum, funcName)

const std::vector<TitleScene::StateFunctionSet>& TitleScene::GetStateTable()
{
	using enum TitleSceneState;
	static const std::vector<StateFunctionSet> stateTable =
	{
		TITLE_SCENE_ENTRY(FADE_IN, FadeIn),
		TITLE_SCENE_ENTRY(READY, Ready),
		TITLE_SCENE_ENTRY(PLAY, Play),
		TITLE_SCENE_ENTRY(PAUSE, Pause),
		TITLE_SCENE_ENTRY(FADE_OUT, FadeOut)
	};
	return stateTable;
}

TitleScene::TitleScene()
{
	stateMachine_.RegisterFromDefaultTable(this);
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	Load();

	bgmHandle_ = gameAudio_->Play("gameBGM", true, SoundCategory::BGM);
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
	text_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.Title.AnchorPoint"));

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
	enemyMgr_.SetBeatAnalyzer(&gameAudio_->GetBeatAnalyzer());
	enemyMgr_.SetIsInGame(false);

	player_->SetEnemyManager(&enemyMgr_);

	oscillator_.SetType(Oscillator::Type::Sine);

	stateMachine_.ChangeState(TitleSceneState::FADE_IN);
}

void TitleScene::Update() {
	// デバッグ用更新処理
	DebugUpdate();

	oscillator_.Update();

	stateMachine_.UpdateState(Timer::GetInstance()->GetDeltaTime());

	// UI更新
	enterSpr_->Update();
	text_->Update();
	operation_->Update();
	reticle_->Update();

	
	// 地面の回転演出
	Vector3 rotate = groundWT_.GetRotate();
	rotate.y -= rotateSpeed_ * Timer::GetInstance()->GetDeltaTime();
	groundWT_.SetRotate(rotate);
	groundWT_.Update();

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
