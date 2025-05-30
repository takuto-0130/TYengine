#include "GameCore.h"
#include "ParticleClass.h"
#include "Audio/Audio.h"
#include "SceneFactory.h"

#include "PlaneParticle.h"
#include "RingParticle.h"
#include "CylinderParticle.h"

//#include "SimpleUpdater.h"
//#include "BillboardRenderer.h"
//#include "ConeEmitter.h"
//#include "AlphaFadeUpdater .h"

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

	//ParticleClass::GetInstance()->Initialize(directXBasis, srvManager.get(), camera.get());
	particleManager = std::make_unique<ParticleManager>();

	auto plane = std::make_unique<PlaneParticle>();
	auto ring = std::make_unique<RingParticle>();
	auto cylinder = std::make_unique<CylinderParticle>();

	//particleManager->Add(std::move(plane));
	//particleManager->Add(std::move(ring));
	particleManager->Add(std::move(cylinder));

	particleManager->InitializeAll(directXBasis, srvManager.get(), camera.get());

	// ParticleManager の作成と初期化
	//particleManager = std::make_unique<ParticleManager>();
	//particleManager->Initialize(directXBasis, srvManager.get());

	//// ParticleSystem の作成
	//auto* system = particleManager->CreateSystem("Test");

	//// エミッタの作成と設定
	//auto emitter = std::make_unique<ConeEmitter>();
	//emitter->transform.translate = { 0, 0, 0 }; // 発生位置
	//emitter->count = 50;                        // 一度に出す数
	////emitter->angle = 360.0f;                    // 全方向に発射
	////emitter->speed = 0.05f;                     // 速度
	//system->SetEmitter(std::move(emitter));

	//// アップデーターの設定
	//system->AddUpdater(std::make_unique<AlphaFadeUpdater>());

	//// レンダラーの設定
	//auto renderer = std::make_unique<BillboardRenderer>(camera.get(), srvManager.get());
	//renderer->Initialize();

	//// テクスチャを読み込み、インデックスを設定
	//TextureManager::GetInstance()->LoadTexture("Resources/circle.png");
	//int texIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath("Resources/circle.png");
	//renderer->SetTextureIndex(texIndex);

	//// レンダラーをシステムに設定
	//system->SetRenderer(std::move(renderer));
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
		particleManager->UpdateAll();
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
	particleManager->DrawAll();

	imgui->Draw();
	directXBasis->DrawEnd();
}
