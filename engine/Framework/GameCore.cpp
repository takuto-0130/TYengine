#include "GameCore.h"
#include "Audio/Audio.h"
#include "SceneFactory.h"

#include "PlaneParticle.h"
#include "RingParticle.h"
#include "CylinderParticle.h"

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

	particleManager = ParticleManager::GetInstance();

	auto plane = std::make_unique<PlaneParticle>();
	auto ring = std::make_unique<RingParticle>();
	auto cylinder = std::make_unique<CylinderParticle>();

	int index = particleManager->Add(std::move(plane));
	int indexRing = particleManager->Add(std::move(ring));
	particleManager->Add(std::move(cylinder));

	particleManager->InitializeAll(directXBasis, srvManager.get(), camera.get());
	IParticleRenderer::Emitter emitter;
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
	emitterRing.count = 4;
	emitterRing.frequency = 1.5f;
	particleManager->SetEmitter(indexRing, emitterRing);



	//particleManager->SetEmitter(indexRing, emitterRing);


	renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(directXBasis, srvManager.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 1,0,0,1 });

	copyPass = std::make_unique<CopyPass>();
	copyPass->Initialize(directXBasis, srvManager.get(), L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/CopyImage.PS.hlsl");
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
		copyPass->Update();
		/// ↓更新処理ここから

		imgui->End();
	}
}

void GameCore::Draw()
{
	// ---------- オフスクリーン描画 ----------
	renderTexture->BeginRender();

	srvManager->BeginDraw(); // SRVマネージャでIDリセットなど
	sceneManager_->Draw();   // 実際の描画

	particleManager->DrawAll();

	renderTexture->EndRender();

	// ---------- SwapChainへの描画 ----------
	directXBasis->DrawBegin();

	copyPass->Draw(directXBasis->GetCommandList(), renderTexture->GetGPUHandle());
	// ImGuiはSwapChainに描く（上書き）
	imgui->Draw();


	directXBasis->DrawEnd();
}
