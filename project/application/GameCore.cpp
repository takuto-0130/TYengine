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


using namespace TYEngine::Framework;
using namespace TYEngine::Core;
using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;
using namespace TYEngine::Debugger;
using namespace TYEngine::CameraSystem;
using namespace TYEngine::Effect;
using namespace TYEngine::OffScreen;
using namespace TYEngine::AudioSystem;

void GameCore::Initialize()
{
	// フレームワーク基盤の初期化（Window, DirectXなど）
	TYFrameWork::Initialize();

	// タイマーの開始
	Timer::GetInstance()->Start();

	// ImGuiマネージャの初期化
	imgui_ = ImGuiManager::GetInstance();
	imgui_->Initialize(windowsApp_.get(), directXBasis_);

	// スプライト基盤の初期化
	spriteBasis_ = SpriteBasis::GetInstance();
	spriteBasis_->Initialize(directXBasis_);

	// カメラの生成
	camera_ = std::make_unique<Camera>();

	// 3Dオブジェクト基盤の初期化とカメラ設定
	object3dBasis_ = Object3dBasis::GetInstance();
	object3dBasis_->Initialize(directXBasis_);
	object3dBasis_->SetDefaultCamera(camera_.get());

	// モデルマネージャの初期化
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize(directXBasis_, srvManager_.get());

	// オーディオシステムの初期化
	Audio::GetInstance()->Initialize();
	GameAudio::GetInstance()->Init();

	// シーンファクトリの生成と初期シーンの設定
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");

	// パーティクルマネージャの初期化と各挙動（ビヘイビア）の設定
	particleManager_ = ParticleManager::GetInstance();

	auto plane = std::make_unique<PlaneParticle>();
	auto ring = std::make_unique<RingParticle>();
	auto cylinder = std::make_unique<CylinderParticle>();

	// リング状爆発エフェクト設定
	ring->SetBehaviour(std::make_unique<ExplosionRingBehaviour>());//

	auto contrail = std::make_unique<PlaneParticle>();  // 板ポリ形状
	contrail->SetBehaviour(std::make_unique<ContrailBehaviour>()); // コントレイル挙動を設定

	auto explosion = std::make_unique<PlaneParticle>(); // 爆発
	explosion->SetBehaviour(std::make_unique<ExplosionBehaviour>());

	auto debris = std::make_unique<PlaneParticle>(); // 破片
	debris->SetBehaviour(std::make_unique<DebrisBehaviour>());

	// パーティクルの登録
	int index = particleManager_->Add(std::move(plane));		// 0
	int indexRing = particleManager_->Add(std::move(ring));	// 1
	particleManager_->Add(std::move(cylinder));				// 2
	particleManager_->Add(std::move(contrail));				// 3
	particleManager_->Add(std::move(explosion));				// 4
	particleManager_->Add(std::move(debris));				// 5

	// 全パーティクルの一括初期化
	particleManager_->InitializeAll(directXBasis_, srvManager_.get(), camera_.get());
	
	// 初期エミッター設定
	IParticleRenderer::Emitter emitter{};
	particleManager_->SetEmitter(index, emitter);

	IParticleRenderer::Emitter emitterRing{};
	particleManager_->SetEmitter(indexRing, emitterRing);


	// コライダーマネージャのインスタンス確保
	ColliderManager::GetInstance();


	// レンダリング用テクスチャの生成（メイン、一時用、アウトライン用）
	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize(directXBasis_, srvManager_.get(), WindowsApp::kClientWidth, WindowsApp::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 1, 0, 0, 1 });
	
	tempTexture_ = std::make_unique<RenderTexture>();
	tempTexture_->Initialize(directXBasis_, srvManager_.get(), WindowsApp::kClientWidth, WindowsApp::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 0, 0, 0, 1 });

	outlineTexture_ = std::make_unique<RenderTexture>();
	outlineTexture_->Initialize(directXBasis_, srvManager_.get(), WindowsApp::kClientWidth, WindowsApp::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 0, 0, 0, 1 });

	// ポストエフェクトマネージャの初期化とレンダーターゲット設定
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
	
	// 初期状態では全エフェクトを無効化
	postEffectManager_->EffectAllDisable();


	// キューブマップ（スカイボックス）基盤の初期化
	CubemapBasis::GetInstance()->Initialize(directXBasis_);
	CubemapBasis::GetInstance()->SetDefaultCamera(camera_.get());
}

void GameCore::Finalize()
{
	// オーディオの解放
	GameAudio::GetInstance()->Destroy();;
	// 基盤システムの終了処理
	TYFrameWork::Finalize();
}

void GameCore::Update()
{
	// Windowsメッセージ処理
	if (windowsApp_->ProcessMessage()) {
		// 終了メッセージなら、ゲームループ脱出フラグを立てる
		endRequest_ = true;
	}
	else { // ゲームループ処理
		// ImGuiフレーム開始
		imgui_->Begin();
		
		// 各種マネージャ・システムの更新
		Timer::GetInstance()->Update();
		GameAudio::GetInstance()->Update();
		Audio::GetInstance()->Update();
		TYFrameWork::Update(); // 入力系などの更新
		particleManager_->UpdateAll();
		camera_->Update();
		postEffectManager_->Update();
		// コライダーの更新（衝突判定など）
		ColliderManager::GetInstance()->Update();
		
		// ImGuiフレーム終了
		imgui_->End();
	}
}

void GameCore::Draw()
{
	// フレームの切り替えタイミングでリングバッファを進める（CommandAllocator切り替え）
	TextureManager::GetInstance()->NextFrame();

	// ---------- オフスクリーン描画（RenderTextureへの描画） ----------
	renderTexture_->BeginRender();

	sceneManager_->Draw();   // シーン内の3Dオブジェクト/スプライト描画
	particleManager_->DrawAll(); // パーティクル描画

	renderTexture_->EndRender();

	// ---------- SwapChain（画面）への描画 ----------
	directXBasis_->DrawBegin();

	// ポストエフェクトを適用してSwapChainに転送
	// 第二引数に nullptr を指定しているため、最終結果はバックバッファに書き込まれる
	postEffectManager_->Apply(renderTexture_.get(), nullptr); 

	// ポストエフェクトの影響を受けないUIを描画
	sceneManager_->UIDraw(); 
	
	// ImGuiの描画
	imgui_->Draw();

	directXBasis_->DrawEnd();
}
