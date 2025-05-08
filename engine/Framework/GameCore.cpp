#include "GameCore.h"
#include "ParticleClass.h"
#include "Audio/Audio.h"
#include "SceneFactory.h"

#include "SimpleUpdater.h"
#include "BillboardRenderer.h"
#include "ConeEmitter.h"
#include "AlphaFadeUpdater .h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void GameCore::Initialize()
{
	TYFrameWork::Initialize();

	imgui = ImGuiManager::GetInstance();
	imgui->Initialize(windowsApp.get(), directXBasis);

	spriteBasis = SpriteBasis::GetInstance();
	spriteBasis->Initialize(directXBasis);

	camera = std::make_unique<Camera>();

	object3dBasis = Object3dBasis::GetInstance();
	object3dBasis->Initialize(directXBasis);
	object3dBasis->SetDefaultCamera(camera.get());

	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(directXBasis, srvManager.get());

	Audio::GetInstance()->Initialize();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");

	//ParticleClass::GetInstance()->Initialize(directXBasis.get(), srvManager.get(), camera.get());

	// ParticleManager の初期化
	particleManager = std::make_unique<ParticleManager>();
	particleManager->Initialize(directXBasis, srvManager.get());

	// パーティクルシステム作成
	auto* system = particleManager->CreateSystem("Test");

	// エミッタ設定（位置や数）
	auto emitter = std::make_unique<ConeEmitter>();
	emitter->transform.translate = { 0, 0, 0 };
	emitter->count = 50;
	system->SetEmitter(std::move(emitter));

	// アップデーター設定（移動、寿命処理）
	system->AddUpdater(std::make_unique<AlphaFadeUpdater>());

	// レンダラー設定（カメラとSRV必須）
	auto renderer = std::make_unique<BillboardRenderer>(camera.get(), srvManager.get());
	TextureManager::GetInstance()->LoadTexture("Resources/circle.png");
	renderer->SetTextureIndex(TextureManager::GetInstance()->GetTextureIndexByFilePath("Resources/circle.png"));
	system->SetRenderer(std::move(renderer));
}

void GameCore::Finalize()
{
	TYFrameWork::Finalize();
}

void GameCore::Update()
{
	// Windowsメッセージ処理
	if (windowsApp->ProcessMessage()) {
		// ゲームループを抜ける
		endRequest_ = true;
	}
	else { //ゲーム処理
		imgui->Begin();
		TYFrameWork::Update();
		//ParticleClass::GetInstance()->Update();
		particleManager->Update(1.0f/60.0f);
		camera->Update();

		/// ↓更新処理ここから

		imgui->End();
	}
}

void GameCore::Draw()
{
	///// 描画処理
	directXBasis->DrawBegin();

	srvManager->BeginDraw();

	// 描画コマンド
	sceneManager_->Draw();
	//ParticleClass::GetInstance()->Draw();
	particleManager->Draw();

	imgui->Draw();
	directXBasis->DrawEnd();
}
