#include "GameScene.h"
#include "TextureManager.h"
#include "SceneManager.h"
#include "mathFunc.h"
#include "SpriteBasis.h"
#include "Object3dBasis.h"
#include "Audio/Audio.h"
#include "Pause/Pause.h"
#include "Result/Result.h"
#include "../../Object/Enemy/Enemy.h"

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

	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	RailEditor::Instance()->Load("Resources/JSON/RailEditor.json");
	controlPoints_ = RailEditor::Instance()->GetControlPoints();
	triggeredFlags_ = std::vector<bool>(controlPoints_.size(), false);
	triggerObjects_.clear();

	const auto& segments = RailEditor::Instance()->GetSegments();
	for (size_t i = 0; i < controlPoints_.size(); ++i)
	{
		if (i < segments.size() && segments[i].triggerEvent)
		{
			triggerObjects_.emplace_back(std::make_unique<TriggerObject>(controlPoints_[i]));
			triggerObjects_.back()->world.TransferMatrix();
		}
	}

	segmentCount = oneSegmentCount * controlPoints_.size();
	SetSegment();
	RailReDraw();
	ResetRailCamera();


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
#ifdef _DEBUG
	isRailCameraMove_ = false;
#else
	isRailCameraMove_ = true;
#endif // _DEBUG

	scoreDraw_ = std::make_unique<score>();
	scoreDraw_->Initialze();

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint({ 0.5f,0.5f });

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/white2x2.png");
	for (size_t i = 0; i < 2; ++i) {
		lasers_[i] = std::make_unique<Sprite>();
		lasers_[i]->Initialize("Resources/Texture/white2x2.png");
		lasers_[i]->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	}

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ComboText.png");
	comboText_ = std::make_unique<Sprite>();
	comboText_->Initialize("Resources/Texture/ComboText.png");
	comboText_->SetAnchorPoint({ 0.5f,0.5f });
	comboText_->SetPosition(offsetComboTextPos_);

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");
	comboNumTex_ = std::make_unique<Sprite>();
	comboNumTex_->Initialize("Resources/Texture/number.png");
	comboNumTex_->SetTextureSize({ 64,64 });
	comboNumTex_->SetTextureLeftTop({ 128,0 });
	comboNumTex_->SetPosition(offsetComboNum_);
	comboNumTex_->SetSize({ 64,64 });

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

	for (const auto& rail : rails_)
	{
		rail->Draw();
	}

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
	if(GetCurrentState() != GameSceneState::FADE_OUT)
	{
		Vector2 mouse = input_->GetMousePosition();
		if (input_->PushKey(DIK_SPACE) && GetCurrentState() == GameSceneState::PLAY)
		{
			for (size_t i = 0; i < 2; ++i)
			{
				lasers_[i]->DrawRect(mouse, mouse,
					{ 426.7f * float(1 + i) - 20.0f, 720 },
					{ 426.7f * float(1 + i) + 20.0f, 720 });
			}
		}
		reticle_->Draw();
		comboNumTex_->Draw();
		comboText_->Draw();

		if (GetCurrentState() == GameSceneState::RESULT)
		{
			resultMenu_->Draw();
		}
		scoreDraw_->Draw();

		if (GetCurrentState() == GameSceneState::PAUSE)
		{
			pauseMenu_->Draw();
		}
	}
}

void GameScene::PlayUIUpdate()
{
	scoreDraw_->Update();
	comboText_->Update();
	comboNumTex_->Update();
	reticle_->Update();
	for (size_t i = 0; i < 2; ++i) {
		lasers_[i]->Update();
	}
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

	Vector2 mouse = input_->GetMousePosition();
	reticle_->SetPosition(mouse);


	float t = comboTimer_ / kComboTime_;
	t = 1.0f - powf(1.0f - t, 4.0f);
	comboText_->SetColor(Vector4(1.0f, 1.0f, 1.0f, t));
	comboNumTex_->SetColor(Vector4(1.0f, 1.0f, 1.0f, t));

	t = (comboTimer_ - (kComboTime_ - shakeTime_)) / (kComboTime_ - (kComboTime_ - shakeTime_));
	if (t > 0)
	{
		std::mt19937 random(seedGene_());
		std::uniform_real_distribution<float> dist(-15.0f, 15.0f);
		Vector2 pos = { dist(random),dist(random) };
		comboText_->SetPosition(offsetComboTextPos_ + pos * t);
		comboNumTex_->SetPosition(offsetComboNum_ + pos * t);
	}
	else
	{
		comboText_->SetPosition(offsetComboTextPos_);
		comboNumTex_->SetPosition(offsetComboNum_);
	}
}

void GameScene::PopRail(Vector3 position, Vector3 rota)
{
	auto rail = std::make_unique<Rail>();
	rail->Initialize(position);
	rail->SetRotate(rota);
	rail->UpdateTransform();
	rails_.push_back(std::move(rail));
}

void GameScene::StageEdit()
{
#ifdef _DEBUG
	RailEditor::Instance()->DrawEditorUI();
	if (RailEditor::Instance()->NeedsPreviewUpdate())
	{
		controlPoints_ = RailEditor::Instance()->GetControlPoints();
		triggeredFlags_ = std::vector<bool>(controlPoints_.size(), false);
		triggerObjects_.clear();

		const auto& segments = RailEditor::Instance()->GetSegments();
		for (size_t i = 0; i < controlPoints_.size(); ++i)
		{
			if (i < segments.size() && segments[i].triggerEvent)
			{
				triggerObjects_.emplace_back(std::make_unique<TriggerObject>(controlPoints_[i]));
				triggerObjects_.back()->world.TransferMatrix();
			}
		}

		segmentCount = oneSegmentCount * controlPoints_.size();
		SetSegment();
		RailReDraw();
		RailEditor::Instance()->ResetPreviewFlag();
	}
	enemyManager_->DrawEditorUI();
#endif
}

void GameScene::RailReDraw()
{
	pointsDrawing_.clear();
	for (size_t i = 0; i < segmentCount + 1; ++i)
	{
		float t = 1.0f / segmentCount * i;
		Vector3 pos = CatmullRomPosition(controlPoints_, t);
		pointsDrawing_.push_back(pos);
	}
	rails_.clear();
	size_t i = 0;
	for (Vector3& v : pointsDrawing_)
	{
		if (pointsDrawing_[i] == pointsDrawing_.back()) break;
		++i;
		Vector3 forward = pointsDrawing_[i] - v;
		Vector3 rotate{};
		rotate.y = std::atan2(forward.x, forward.z);
		float len = Length({ forward.x, 0, forward.z });
		rotate.x = std::atan2(-forward.y, len);
		PopRail(v, rotate);
	}
}

void GameScene::RailCameraMove()
{
	if (cameraForwardT <= 1.0f)
	{
		cameraEyeT += cameraSegmentCount;
		cameraForwardT += cameraSegmentCount;
		Vector3 eye = CatmullRomPosition(controlPoints_, cameraEyeT);
		eye.y += 0.5f;
		camera_->SetTranslate(eye);

		Vector3 forward = CatmullRomPosition(controlPoints_, cameraForwardT);
		forward.y += 0.5f;
		forward = forward - eye;

		if (cameraForwardT <= 1.0f)
		{
			Vector3 rot{};
			rot.y = std::atan2(forward.x, forward.z);
			float len = Length({ forward.x, 0, forward.z });
			rot.x = std::atan2(-forward.y, len);
			camera_->SetRotate(rot);
		}

		size_t currentIndex = static_cast<size_t>(cameraEyeT * controlPoints_.size());
		const auto& segments = RailEditor::Instance()->GetSegments();
		if (currentIndex < segments.size() && segments[currentIndex].triggerEvent)
		{
			if (alreadyTriggeredIndices_.find(currentIndex) == alreadyTriggeredIndices_.end())
			{
				enemyManager_->TriggerNextEnemyGroup();
				//Audio::GetInstance()->PlayWave("fanfare");
				alreadyTriggeredIndices_.insert(currentIndex);
			}
		}
	}
	else
	{
		isRailCameraMove_ = false;
#ifndef _DEBUG
		ChangeState(GameSceneState::RESULT);
#endif // !_DEBUG
	}
}

void GameScene::RailCameraDebug()
{
#ifdef _DEBUG
	ImGui::Begin("RailCamera");
	if (ImGui::Button("Start/Stop"))
	{
		isRailCameraMove_ = !isRailCameraMove_;
	}
	if (ImGui::Button("ResetCamera"))
	{
		ResetRailCamera();
		RailCameraMove();
	}
	if (isRailCameraMove_)
	{
		RailCameraMove();
	}
	ImGui::Text("eye%.03f", cameraEyeT);
	ImGui::Text("forward%.03f", cameraForwardT);
	ImGui::End();
#endif
}

void GameScene::SetSegment()
{
	float denom = kDivisionSpan * controlPoints_.size();
	cameraSegmentCount = 1.0f / denom;
}

void GameScene::ResetRailCamera()
{
	float denom = kDivisionSpan * controlPoints_.size();
	cameraEyeT = 0;
	cameraForwardT = 30.0f / denom;
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
			comboNumTex_->SetTextureLeftTop({ 64.0f * float(comboCount_),0 });
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
