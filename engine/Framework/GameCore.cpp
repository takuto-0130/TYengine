#include "GameCore.h"
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
	particleManager = ParticleManager::GetInstance();

	auto plane = std::make_unique<PlaneParticle>();
	auto ring = std::make_unique<RingParticle>();
	auto cylinder = std::make_unique<CylinderParticle>();

	/*int index = particleManager->Add(std::move(plane));
	int indexRing = particleManager->Add(std::move(ring));*/
	particleManager->Add(std::move(cylinder));

	particleManager->InitializeAll(directXBasis, srvManager.get(), camera.get());
	/*IParticleRenderer::Emitter emitter;
	emitter.transform.scale = { 0.05f,1.0f,1.0f };
	emitter.transform.rotate = { 0,0,0 };
	emitter.transform.translate = { 0,0,0 };
	emitter.count = 5;
	emitter.frequency = 1.5f;
	particleManager->SetEmitter(index, emitter);

	IParticleRenderer::Emitter emitterRing;
	emitterRing.transform.scale = { 0.5f,0.5f,0.5f };
	emitterRing.transform.rotate = { 0,0,0 };
	emitterRing.transform.translate = { 0,0,0 };
	emitterRing.count = 1;
	emitterRing.frequency = 1.5f;
	particleManager->SetEmitter(indexRing, emitterRing);*/
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
