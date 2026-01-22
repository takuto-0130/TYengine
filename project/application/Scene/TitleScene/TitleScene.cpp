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
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

#include "PostEffectManager.h"
#include "VignetteEffect.h"
#include "RadialBlurEffect.h"


TitleScene::~TitleScene()
{
}

static std::vector<float> spectrumSmoothed(32, 0.0f);
// Nバンドにリサンプルしたスペクトラムを作る
std::vector<float> MakeLogSpectrum(
	const std::vector<float>& fft,
	int sampleRate,
	int fftSize,
	int bands)
{
	std::vector<float> out(bands, 0.0f);

	float minF = 20.0f;              // 人間の可聴最低周波数
	float maxF = sampleRate / 2.0f;  // ナイキスト

	float minLog = log10f(minF);
	float maxLog = log10f(maxF);

	float freqPerBin = (float)sampleRate / fftSize;

	for (int b = 0; b < bands; b++)
	{
		float logStart = minLog + (maxLog - minLog) * (float)b / bands;
		float logEnd = minLog + (maxLog - minLog) * (float)(b + 1) / bands;

		float fStart = powf(10.0f, logStart);
		float fEnd = powf(10.0f, logEnd);

		int binStart = (int)(fStart / freqPerBin);
		int binEnd = (int)(fEnd / freqPerBin);

		if (binStart < 0) binStart = 0;
		if (binEnd >= fftSize / 2) binEnd = fftSize / 2;

		float sum = 0;
		int count = 0;

		for (int i = binStart; i <= binEnd; i++)
		{
			float v = fft[i];
			v = log10f(1.0f + v * 5.0f);
			sum += v;
			count++;
		}

		out[b] = (count > 0) ? sum / count : 0.0f;
	}

	return out;
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
		int a = Audio::GetInstance()->PlayWave("gameBGM", true);
		Audio::GetInstance()->SetBGMVolume(a, 1.0f);
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
