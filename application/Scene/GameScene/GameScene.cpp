#include "GameScene.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include "Audio/Audio.h"
#include "Pause/Pause.h"
#include "Result/Result.h"
#include "PlayUI/PlayUI.h"
#include "CubemapBasis.h"

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

	stageManager_ = std::make_unique<StageManager>();
	stageManager_->SetCamera(camera_);
	stageManager_->Init();

	EmitterInit();

	scoreDraw_ = std::make_unique<Score>();
	scoreDraw_->Initialze();

	playUI_ = std::make_unique<PlayUI>();
	playUI_->SetScoreDraw(scoreDraw_.get());
	playUI_->Init();

	pauseMenu_ = std::make_unique<PauseClass>();
	pauseMenu_->Initialze();

	resultMenu_ = std::make_unique<ResultClass>();
	resultMenu_->Initialze();

	ChangeState(GameSceneState::FADE_IN);
}

void GameScene::Update()
{
	UpdateState(1.0f / 60.0f);
	
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

	// フェード中は描画しない
	if (GetCurrentState() != GameSceneState::FADE_OUT && GetCurrentState() != GameSceneState::FADE_IN)
	{
		if (GetCurrentState() != GameSceneState::RESULT) playUI_->Draw();

		if (GetCurrentState() == GameSceneState::RESULT) resultMenu_->Draw(), scoreDraw_->Draw();

		if (GetCurrentState() == GameSceneState::PAUSE) pauseMenu_->Draw();
	}
}
