#include "GameCore.h"
#include "Audio.h"
#include "Scene/SceneFactory/SceneFactory.h"
#include "ColliderManager.h"
#include "CubemapBasis.h"
#include "Timer.h"
#include "Utils/Json/JsonManager.h"

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
#pragma region // フレームワーク基盤・主要マネージャの初期化
	// 1. フレームワーク基盤の初期化（Window, DirectXなど）
	TYFramework::Initialize();

	// 2. タイマーの開始
	Timer::GetInstance()->Start();

	// 3. ImGuiマネージャの初期化
	imgui_ = ImGuiManager::GetInstance();
	imgui_->Initialize(windowsApp_.get(), directXBasis_);

	// 4. 2Dスプライト基盤の初期化
	spriteBasis_ = SpriteBasis::GetInstance();
	spriteBasis_->Initialize(directXBasis_);

	// 5. メインカメラの生成
	camera_ = std::make_unique<Camera>();

	// 6. 3Dオブジェクト基盤の初期化とカメラ設定
	object3dBasis_ = Object3dBasis::GetInstance();
	object3dBasis_->Initialize(directXBasis_);
	object3dBasis_->SetDefaultCamera(camera_.get());

	// 7. 3Dモデルマネージャの初期化
	modelManager_ = ModelManager::GetInstance();
	modelManager_->Initialize(directXBasis_, srvManager_.get());

	// 8. オーディオシステムの初期化
	Audio::GetInstance()->Initialize();
	GameAudio::GetInstance()->Init();

	// 9. シーンファクトリの生成と初期シーン（タイトル）の設定
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");
#pragma endregion

#pragma region // パーティクルシステムの構築・登録
	particleManager_ = ParticleManager::GetInstance();

	auto plane = std::make_unique<PlaneParticle>();
	auto ring = std::make_unique<RingParticle>();
	auto cylinder = std::make_unique<CylinderParticle>();

	// 各種エフェクト挙動（ビヘイビア）の設定
	ring->SetBehaviour(std::make_unique<ExplosionRingBehaviour>());

	auto contrail = std::make_unique<PlaneParticle>();
	contrail->SetBehaviour(std::make_unique<ContrailBehaviour>());

	auto explosion = std::make_unique<PlaneParticle>();
	explosion->SetBehaviour(std::make_unique<ExplosionBehaviour>());

	auto debris = std::make_unique<PlaneParticle>();
	debris->SetBehaviour(std::make_unique<DebrisBehaviour>());

	// パーティクルタイプリストへの登録
	int index = particleManager_->Add(std::move(plane));	// 0: Normal
	int indexRing = particleManager_->Add(std::move(ring));	// 1: Ring
	particleManager_->Add(std::move(cylinder));				// 2: Cylinder
	particleManager_->Add(std::move(contrail));				// 3: Contrail
	particleManager_->Add(std::move(explosion));			// 4: Explosion
	particleManager_->Add(std::move(debris));				// 5: Debris

	// 全パーティクルの一括初期化とエミッター初期値設定
	particleManager_->InitializeAll(directXBasis_, srvManager_.get(), camera_.get());
	
	IParticleRenderer::Emitter emitter{};
	particleManager_->SetEmitter(index, emitter);

	IParticleRenderer::Emitter emitterRing{};
	particleManager_->SetEmitter(indexRing, emitterRing);
#pragma endregion

#pragma region // 衝突判定・オフスクリーンレンダーターゲット・ポストエフェクトの構築
	// コライダーマネージャのインスタンス確保
	ColliderManager::GetInstance();

	// 解像度・クリアカラー設定のロード
	JsonManager engineJM;
	engineJM.Load("EngineConfig.json");
	uint32_t rtWidth = engineJM.Get<uint32_t>("window.width", WindowsApp::kClientWidth);
	uint32_t rtHeight = engineJM.Get<uint32_t>("window.height", WindowsApp::kClientHeight);
	Vector4 mainClearColor = engineJM.Get<Vector4>("graphics.mainClearColor", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	Vector4 defaultClearColor = engineJM.Get<Vector4>("graphics.clearColor", Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	// レンダリング用テクスチャの生成（メイン、一時用、アウトライン用）
	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize(directXBasis_, srvManager_.get(), rtWidth, rtHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, mainClearColor);
	
	tempTexture_ = std::make_unique<RenderTexture>();
	tempTexture_->Initialize(directXBasis_, srvManager_.get(), rtWidth, rtHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, defaultClearColor);

	outlineTexture_ = std::make_unique<RenderTexture>();
	outlineTexture_->Initialize(directXBasis_, srvManager_.get(), rtWidth, rtHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, defaultClearColor);

	// ポストエフェクトマネージャの初期化とレンダーターゲット設定
	postEffectManager_ = PostEffectManager::GetInstance();
	postEffectManager_->Initialize(directXBasis_, srvManager_.get());
	postEffectManager_->SetTempRenderTexture(std::move(tempTexture_));
	postEffectManager_->SetOutlineRenderTexture(std::move(outlineTexture_));

	// 適用する全ポストエフェクトの追加
	postEffectManager_->AddEffect("LuminanceBasedOutline", std::make_unique<LuminanceBasedOutlineEffect>());
	postEffectManager_->AddEffect("Grayscale", std::make_unique<GrayscaleEffect>());
	postEffectManager_->AddEffect("Vignette", std::make_unique<VignetteEffect>());
	postEffectManager_->AddEffect("BoxFilter", std::make_unique<BoxFilterEffect>());
	postEffectManager_->AddEffect("Gaussian", std::make_unique<GaussianEffect>());
	postEffectManager_->AddEffect("RadialBlur", std::make_unique<RadialBlurEffect>());
	postEffectManager_->AddEffect("Random", std::make_unique<RandomEffect>());
	postEffectManager_->AddEffect("Dissolve", std::make_unique<DissolveEffect>());
	postEffectManager_->AddEffect("HealthVignette", std::make_unique<VignetteEffect>());
	
	// 初期状態では全エフェクトを無効化
	postEffectManager_->EffectAllDisable();

	// キューブマップ（スカイボックス）基盤の初期化
	CubemapBasis::GetInstance()->Initialize(directXBasis_);
	CubemapBasis::GetInstance()->SetDefaultCamera(camera_.get());
#pragma endregion
}

void GameCore::Finalize()
{
	// オーディオの解放
	GameAudio::GetInstance()->Destroy();;
	// 基盤システムの終了処理
	TYFramework::Finalize();
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
		TYFramework::Update(); // 入力系などの更新
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
