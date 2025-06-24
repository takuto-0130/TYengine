#include "GameScene.h"
#include "mathFunc.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include "Audio/Audio.h"
#include "Pause/Pause.h"
#include "Result/Result.h"
#include "../../Object/Enemy/Enemy.h"
#include "../../Object/Rail/RailManager.h"
#include "./PlayUI/PlayUI.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

const std::vector<StateMachine<GameScene, GameSceneState>::StateFunctionSet>& GameScene::GetStateTable()
{
	static const std::vector<StateFunctionSet> stateTable = {
	{ GameSceneState::LOAD,		   &GameScene::InitLoad,		&GameScene::UpdateLoad,		  &GameScene::ExitLoad },
	{ GameSceneState::FADE_IN,	   &GameScene::InitFadeIn,		&GameScene::UpdateFadeIn,	  &GameScene::ExitFadeIn },
	{ GameSceneState::READY,	   &GameScene::InitReady,		&GameScene::UpdateReady,	  &GameScene::ExitReady },
	{ GameSceneState::PLAY,		   &GameScene::InitPlay,		&GameScene::UpdatePlay,		  &GameScene::ExitPlay },
	{ GameSceneState::PAUSE,	   &GameScene::InitPause,		&GameScene::UpdatePause,	  &GameScene::ExitPause },
	{ GameSceneState::DEAD,		   &GameScene::InitDead,		&GameScene::UpdateDead,		  &GameScene::ExitDead },
	{ GameSceneState::RESULT,	   &GameScene::InitResult,		&GameScene::UpdateResult,	  &GameScene::ExitResult },
	{ GameSceneState::RETRY,	   &GameScene::InitRetry,		&GameScene::UpdateRetry,	  &GameScene::ExitRetry },
	{ GameSceneState::FADE_OUT,	   &GameScene::InitFadeOut,		&GameScene::UpdateFadeOut,	  &GameScene::ExitFadeOut },
	{ GameSceneState::DEBUG_EDIT,  &GameScene::InitDebugEdit,	&GameScene::UpdateDebugEdit,  &GameScene::ExitDebugEdit },
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
	
	Audio::GetInstance()->LoadWave("fanfare");

	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Init();

	railManager_ = std::make_unique<RailManager>();
	railManager_->SetCamera(camera_);
	railManager_->Init();

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();


	emitter.transform.scale = { 0.05f,1.0f,1.0f };
	emitter.transform.rotate = { 0,0,0 };
	emitter.transform.translate = { 0,0,0 };
	emitter.count = 5;
	emitter.frequency = 1.5f;

	emitterRing.transform.scale = { 0.5f,0.5f,0.5f };
	emitterRing.transform.rotate = { 0,0,0 };
	emitterRing.transform.translate = { 0,0,0 };
	emitterRing.count = 1;
	emitterRing.frequency = 1.5f;

	playUI_ = std::make_unique<PlayUI>();
	playUI_->Init();

	scoreDraw_ = std::make_unique<score>();
	scoreDraw_->Initialze();

	pauseMenu_ = std::make_unique<Pause>();
	pauseMenu_->Initialze();

	resultMenu_ = std::make_unique<Result>();
	resultMenu_->Initialze();

	ChangeState(GameSceneState::FADE_IN);
}

void GameScene::Update()
{
	skydome_->Update();

	UpdateState(1.0f / 60.0f);

	if (false) {
		enemyManager_->TriggerNextEnemyGroup();
	}

#ifdef _DEBUG
	ImGui::Begin("Play : Editor Switch");
	if(otherEditorSwitch_)
	{
		ImGui::Checkbox("Other Editor Switch", &otherEditorSwitch_);
		if (!otherEditorSwitch_) ChangeState(GameSceneState::PLAY);
	}
	else
	{
		ImGui::Checkbox("Other Editor Switch", &otherEditorSwitch_);
		if (otherEditorSwitch_) ChangeState(GameSceneState::DEBUG_EDIT);
	}
	ImGui::End();
#endif // _DEBUG
	
}

void GameScene::Draw()
{
	Object3dBasis::GetInstance()->BasisDrawSetting();
	skydome_->Draw();

	railManager_->Draw();

	if (otherEditorSwitch_) {
#ifdef _DEBUG
		enemyManager_->DrawEditorEnemies();
#endif // _DEBUG
	}
	else
	{
		enemyManager_->Draw();
	}

	SpriteBasis::GetInstance()->BasisDrawSetting();

	// フェードアウト中は描画しない
	if (GetCurrentState() != GameSceneState::FADE_OUT)
	{
		playUI_->Draw();

		if (GetCurrentState() == GameSceneState::RESULT) resultMenu_->Draw();

		scoreDraw_->Draw();

		if (GetCurrentState() == GameSceneState::PAUSE) pauseMenu_->Draw();
	}
}

void GameScene::PlayUIUpdate()
{
	scoreDraw_->Update();
	playUI_->Update();
}

void GameScene::AttackUpdate()
{
	if (input_->PushKey(DIK_SPACE)) Collision();

	if (comboTimer_ > 0)
	{
		comboTimer_ -= 1.0f / 60.0f;
		if (comboTimer_ < 0)
		{
			comboTimer_ = 0;
		}
	}
	playUI_->SetComboTime(kComboTime_);
	playUI_->SetComboTimer(comboTimer_);
	playUI_->ComboTexUpdate();
}

void GameScene::StageEdit()
{
#ifdef _DEBUG
	railManager_->StageEdit();
	enemyManager_->DrawEditorUI();
#endif
}

void GameScene::Collision()
{
	int i = 0;
	for (auto& enemy : enemyManager_->GetActiveEnemies()) {
		Vector3 pos = enemy->GetWorldPosition();
		Matrix4x4 matView = MakeViewportMatrix(0, 0, WindowsApp::kClientWidth, WindowsApp::kClientHieght, 0, 1);
		Matrix4x4 matVPV = camera_->GetViewMatrix() * camera_->GetProjectionMatrix() * matView;
		pos = TransformM(pos, matVPV);
		Vector2 mouse = input_->GetMousePosition();

		if (Length(Vector2{ pos.x, pos.y } - Vector2{ mouse.x, mouse.y }) <= 50.0f && !enemy->IsDead()) {

			if (comboTimer_ <= 0) comboCount_ = 0;

			comboCount_++;
			playUI_->SetComboNum(comboCount_);
			score_ += kBasicScore_ * comboCount_;
			scoreDraw_->SetScore(score_);
			comboTimer_ = kComboTime_;
			enemy->OnCollision();
			emitter.transform.translate = enemy->GetWorldPosition();
			emitter.count = comboCount_ + 2;
			emitterRing.transform.translate = emitter.transform.translate;
			emitterRing.count = comboCount_;
			ParticleManager::GetInstance()->SetEmitter(0, emitter);
			ParticleManager::GetInstance()->TriggerEmit(0, true);
			ParticleManager::GetInstance()->SetEmitter(1, emitterRing);
			ParticleManager::GetInstance()->TriggerEmit(1, true);
		}
		i++;
	}
}
