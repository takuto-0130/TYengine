#include "GameCore.h"
#include "Audio/Audio.h"
#include "SceneFactory.h"
#include "ColliderManager.h"
#include "CubemapBasis.h"
#include "Timer.h"

#include "PlaneParticle.h"
#include "RingParticle.h"
#include "CylinderParticle.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void GameCore::Initialize()
{
	TYFrameWork::Initialize();

	Timer::GetInstance()->Start();

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
	IParticleRenderer::Emitter emitter{};
	particleManager->SetEmitter(index, emitter);

	IParticleRenderer::Emitter emitterRing{};
	particleManager->SetEmitter(indexRing, emitterRing);

	ColliderManager::GetInstance();


	renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(directXBasis, srvManager.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 1,0,0,1 });

	copyPass = std::make_unique<CopyPass>();
	copyPass->Initialize(directXBasis, srvManager.get(), L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/CopyImage.PS.hlsl");


	CubemapBasis::GetInstance()->Initialize(directXBasis);
	CubemapBasis::GetInstance()->SetDefaultCamera(camera.get());
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
		Timer::GetInstance()->Update();
		TYFrameWork::Update();
		particleManager->UpdateAll();
		camera->Update();
		copyPass->Update();
		ColliderManager::GetInstance()->Update();
		imgui->End();
	}
}

void GameCore::Draw()
{
	// フレームの切り替えタイミングでリングバッファを進める
	TextureManager::GetInstance()->NextFrame();

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
