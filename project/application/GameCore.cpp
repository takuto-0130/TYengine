#include "GameCore.h"
#include "Audio/Audio.h"
#include "Scene/SceneFactory/SceneFactory.h"
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

#include "AppSystem/Audio/GameAudio.h"

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void GameCore::Initialize()
{
	TYFrameWork::Initialize();

	Timer::GetInstance()->Start();

	imgui_ = ImGuiManager::GetInstance();
	imgui_->Initialize(windowsApp_.get(), directXBasis_);

	spriteBasis_ = SpriteBasis::GetInstance();
	spriteBasis_->Initialize(directXBasis_);

	camera_ = std::make_unique<Camera>();

	object3dBasis_ = Object3dBasis::GetInstance();
	object3dBasis_->Initialize(directXBasis_);
	object3dBasis_->SetDefaultCamera(camera_.get());

	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize(directXBasis_, srvManager_.get());

	Audio::GetInstance()->Initialize();
	GameAudio::GetInstance()->Init();
	//Audio::GetInstance()->Start();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");

	particleManager_ = ParticleManager::GetInstance();

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

	int index = particleManager_->Add(std::move(plane));		// 0
	int indexRing = particleManager_->Add(std::move(ring));	// 1
	particleManager_->Add(std::move(cylinder));				// 2
	particleManager_->Add(std::move(contrail));				// 3
	particleManager_->Add(std::move(explosion));				// 4
	particleManager_->Add(std::move(debris));				// 5

	particleManager_->InitializeAll(directXBasis_, srvManager_.get(), camera_.get());
	IParticleRenderer::Emitter emitter{};
	particleManager_->SetEmitter(index, emitter);

	IParticleRenderer::Emitter emitterRing{};
	particleManager_->SetEmitter(indexRing, emitterRing);


	ColliderManager::GetInstance();


	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize(directXBasis_, srvManager_.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 1, 0, 0, 1 }); 
	
	tempTexture_ = std::make_unique<RenderTexture>();
	tempTexture_->Initialize(directXBasis_, srvManager_.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 0, 0, 0, 1 });

	outlineTexture_ = std::make_unique<RenderTexture>();
	outlineTexture_->Initialize(directXBasis_, srvManager_.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 0, 0, 0, 1 });

	postEffectManager_ = PostEffectManager::GetInstance();
	postEffectManager_->Initialize(directXBasis_, srvManager_.get());
	postEffectManager_->SetTempRenderTexture(std::move(tempTexture_));
	postEffectManager_->SetOutlineRenderTexture(std::move(outlineTexture_));

	// 適用するエフェクトを追加（順番に処理される）
	postEffectManager_->AddEffect("LuminanceBasedOutline", std::make_unique<LuminanceBasedOutlineEffect>());
	postEffectManager_->AddEffect("Grayscale", std::make_unique<GrayscaleEffect>());
	postEffectManager_->AddEffect("Vignette", std::make_unique<VignetteEffect>());
	postEffectManager_->AddEffect("BoxFilter", std::make_unique<BoxFilterEffect>());
	postEffectManager_->AddEffect("Gaussian", std::make_unique<GaussianEffect>());
	postEffectManager_->AddEffect("RadialBlur", std::make_unique<RadialBlurEffect>());
	postEffectManager_->AddEffect("Random", std::make_unique<RandomEffect>());
	postEffectManager_->AddEffect("Dissolve", std::make_unique<DissolveEffect>());
	
	postEffectManager_->EffectAllDisable();


	CubemapBasis::GetInstance()->Initialize(directXBasis_);
	CubemapBasis::GetInstance()->SetDefaultCamera(camera_.get());
}

void GameCore::Finalize()
{
	GameAudio::GetInstance()->Destroy();;
	TYFrameWork::Finalize();
}

void GameCore::Update()
{
	// Windowsメッセージ処理
	if (windowsApp_->ProcessMessage()) {
		// ゲームループを抜ける
		endRequest_ = true;
	}
	else { //ゲーム処理
		imgui_->Begin();
		Timer::GetInstance()->Update();
		Audio::GetInstance()->Update();
		TYFrameWork::Update();
		particleManager_->UpdateAll();
		camera_->Update();
		postEffectManager_->Update();
		ColliderManager::GetInstance()->Update();
		imgui_->End();
	}
}

void GameCore::Draw()
{
	// フレームの切り替えタイミングでリングバッファを進める
	TextureManager::GetInstance()->NextFrame();

	// ---------- オフスクリーン描画 ----------
	renderTexture_->BeginRender();

	sceneManager_->Draw();   // 実際の描画

	particleManager_->DrawAll();

	renderTexture_->EndRender();

	// ---------- SwapChainへの描画 ----------
	directXBasis_->DrawBegin();

	postEffectManager_->Apply(renderTexture_.get(), nullptr); // nullptr指定でSwapChain描画

	sceneManager_->UIDraw(); // ポストエフェクト後にUIを描画
	
	imgui_->Draw();


	directXBasis_->DrawEnd();
}
