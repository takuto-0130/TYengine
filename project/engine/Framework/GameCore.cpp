#include "GameCore.h"
#include "Audio/Audio.h"
#include "SceneFactory.h"
#include "ColliderManager.h"
#include "CubemapBasis.h"
#include "Timer.h"

#include "PlaneParticle.h"
#include "RingParticle.h"
#include "CylinderParticle.h"
#include "Effect/ContrailBehaviour.h"
#include "Effect/ExplosionBehaviour.h"
#include "Effect/ExplosionRingBehaviour.h"
#include "Effect/DebrisBehaviour.h"

#include "GrayscaleEffect.h"
#include "VignetteEffect.h"
#include "BoxFilterEffect.h"
#include "GaussianEffect.h"
#include "RadialBlurEffect.h"
#include "LuminanceBasedOutlineEffect.h"
#include "DissolveEffect.h"
#include "RandomEffect.h"

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


	ring->SetBehaviour(std::make_unique<ExplosionRingBehaviour>());//

	auto contrail = std::make_unique<PlaneParticle>();  // 板ポリ形状
	contrail->SetBehaviour(std::make_unique<ContrailBehaviour>()); // 挙動を設定

	auto explosion = std::make_unique<PlaneParticle>(); // 爆発
	explosion->SetBehaviour(std::make_unique<ExplosionBehaviour>());

	auto debris = std::make_unique<PlaneParticle>(); // 破片
	debris->SetBehaviour(std::make_unique<DebrisBehaviour>());

	int index = particleManager->Add(std::move(plane));		// 0
	int indexRing = particleManager->Add(std::move(ring));	// 1
	particleManager->Add(std::move(cylinder));				// 2
	particleManager->Add(std::move(contrail));				// 3
	particleManager->Add(std::move(explosion));				// 4
	particleManager->Add(std::move(debris));				// 5

	particleManager->InitializeAll(directXBasis, srvManager.get(), camera.get());
	IParticleRenderer::Emitter emitter{};
	particleManager->SetEmitter(index, emitter);

	IParticleRenderer::Emitter emitterRing{};
	particleManager->SetEmitter(indexRing, emitterRing);


	ColliderManager::GetInstance();


	renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(directXBasis, srvManager.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 1, 0, 0, 1 }); 
	
	tempTexture = std::make_unique<RenderTexture>();
	tempTexture->Initialize(directXBasis, srvManager.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 0, 0, 0, 1 });

	outlineTexture = std::make_unique<RenderTexture>();
	outlineTexture->Initialize(directXBasis, srvManager.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 0, 0, 0, 1 });

	postEffectManager = PostEffectManager::GetInstance();
	postEffectManager->Initialize(directXBasis, srvManager.get());
	postEffectManager->SetTempRenderTexture(std::move(tempTexture));
	postEffectManager->SetOutlineRenderTexture(std::move(outlineTexture));

	// 適用するエフェクトを追加（順番に処理される）
	postEffectManager->AddEffect("LuminanceBasedOutline", std::make_unique<LuminanceBasedOutlineEffect>());
	postEffectManager->AddEffect("Grayscale", std::make_unique<GrayscaleEffect>());
	postEffectManager->AddEffect("Vignette", std::make_unique<VignetteEffect>());
	postEffectManager->AddEffect("BoxFilter", std::make_unique<BoxFilterEffect>());
	postEffectManager->AddEffect("Gaussian", std::make_unique<GaussianEffect>());
	postEffectManager->AddEffect("RadialBlur", std::make_unique<RadialBlurEffect>());
	postEffectManager->AddEffect("Random", std::make_unique<RandomEffect>());
	postEffectManager->AddEffect("Dissolve", std::make_unique<DissolveEffect>());
	
	postEffectManager->EffectAllDisable();


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
		Audio::GetInstance()->Update();
		TYFrameWork::Update();
		particleManager->UpdateAll();
		camera->Update();
		postEffectManager->Update();
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

	sceneManager_->Draw();   // 実際の描画

	particleManager->DrawAll();

	renderTexture->EndRender();

	// ---------- SwapChainへの描画 ----------
	directXBasis->DrawBegin();

	postEffectManager->Apply(renderTexture.get(), nullptr); // nullptr指定でSwapChain描画

	sceneManager_->UIDraw(); // ポストエフェクト後にUIを描画
	
	imgui->Draw();


	directXBasis->DrawEnd();
}
