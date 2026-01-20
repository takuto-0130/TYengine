#include "GameScene.h"
#include "WindowsApp.h"
#include "Object3dBasis.h"
#include "Audio/Audio.h"
#include "CubemapBasis.h"
#include "Result/Result.h"
#include "Pause/Pause.h"
#include "PlayUI/PlayUI.h"
#include "ScoreUI/ScoreUI.h"
#include "StartUI/StartUI.h"
#include "RetryUI/RetryUI.h"
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
	static const std::vector<StateFunctionSet> stateTable =
	{
		GAME_SCENE_ENTRY(LOAD, Load),
		GAME_SCENE_ENTRY(FADE_IN, FadeIn),
		GAME_SCENE_ENTRY(READY, Ready),
		GAME_SCENE_ENTRY(PLAY, Play),
		GAME_SCENE_ENTRY(PAUSE, Pause),
		GAME_SCENE_ENTRY(DEAD, Dead),
		GAME_SCENE_ENTRY(CLEAR, Clear),
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
	//========== ロード ==========//

	gameUIJM_.Load("GameUI.json", true, &err_);
#ifdef _DEBUG
	Logger::Log(err_);
#endif // _DEBUG

	input_ = Input::GetInstance();
	camera_ = Object3dBasis::GetInstance()->GetDefaultCamera();

	Object3dBasis::GetInstance()->SetSkyboxFilePath("Resources/Texture/output_skybox.dds");
	skybox_ = std::make_unique<ObjectCubemap>();
	skybox_->Initialize("Resources/Texture/output_skybox.dds");

	Audio::GetInstance()->LoadWave("fanfare");

	stageManager_ = std::make_unique<StageManager>(camera_);
	stageManager_->Init();

	startCameraPos_ = camera_->GetPosition();
	startCameraRot_ = camera_->GetRotate();
	isReady_ = true;
	readyCount_ = 0;

	EmitterInit();
	UIInit();
	ChangeState(GameSceneState::FADE_IN);

	bulletTime_ = BulletTimeController::GetInstance();
}

void GameScene::Update()
{
#ifdef _DEBUG
	ImGui::Begin("GameScene State Debug");
	DebugImGui("GameScene");
	ImGui::End();

	// ImGui で編集
	ImGui::Begin("JSON Editor");
	static jx::JsonImGuiEditor inspector(gameUIJM_);
	inspector.Draw(gameUIJM_.Root(), "GameUI.json");
	if (ImGui::Button("Save")) gameUIJM_.Save();
	ImGui::End();
#endif // _DEBUG

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
}
