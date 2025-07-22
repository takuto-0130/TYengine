#include "GameScene.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include "Audio/Audio.h"
#include "CubemapBasis.h"
#include "Result/Result.h"
#include "Pause/Pause.h"
#include "PlayUI/PlayUI.h"
#include "ScoreUI/ScoreUI.h"
#include "BulletTimeController.h"
#include "Timer.h"
#include "Ease.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

#define GAME_SCENE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(GameScene, stateEnum, funcName)

const std::vector<StateMachine<GameScene, GameSceneState>::StateFunctionSet>& GameScene::GetStateTable()
{
	using enum GameSceneState;
	static const std::vector<StateFunctionSet> stateTable = {
		GAME_SCENE_ENTRY(LOAD, Load),
		GAME_SCENE_ENTRY(FADE_IN, FadeIn),
		GAME_SCENE_ENTRY(READY, Ready),
		GAME_SCENE_ENTRY(PLAY, Play),
		GAME_SCENE_ENTRY(PAUSE, Pause),
		GAME_SCENE_ENTRY(DEAD, Dead),
		GAME_SCENE_ENTRY(RESULT, Result),
		GAME_SCENE_ENTRY(RETRY, Retry),
		GAME_SCENE_ENTRY(FADE_OUT, FadeOut),
		GAME_SCENE_ENTRY(DEBUG_EDIT, DebugEdit),
	};
	return stateTable;
}

GameScene::GameScene()
{
	RegisterFromDefaultTable(this);
}

GameScene::~GameScene()
{
	Audio::GetInstance()->StopStreaming();
}

void GameScene::Init()
{
	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();

	Object3dBasis::GetInstance()->SetSkyboxFilePath("Resources/Texture/rostock_laage_airport_4k.dds");
	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/rostock_laage_airport_4k.dds");

	Audio::GetInstance()->LoadWave("fanfare");

	stageManager_ = std::make_unique<StageManager>(camera_);
	stageManager_->Init();

	EmitterInit();
	UIInit();
	ChangeState(GameSceneState::FADE_IN);

	bulletTime_ = std::make_unique<BulletTimeController>();
}

void GameScene::Update()
{
	if (input_->TriggerKey(DIK_3))
	{
		bulletTime_->Trigger(0.05f, 0.5f, 2.0f, 0.8f,
			EaseFixed::InQuart, EaseFixed::OutQuart);
	}

	bulletTime_->Update();

	UpdateState(Timer::GetInstance()->GetDeltaTime());
	
	SwitchEdit();

	skybox_->Update();
}

void GameScene::Draw()
{
	CubemapBasis::GetInstance()->DrawBegin();
	skybox_->Draw();

	Object3dBasis::GetInstance()->BasisDrawSetting();
	stageManager_->Draw();

	SpriteBasis::GetInstance()->BasisDrawSetting();
	UIDraw();
}
