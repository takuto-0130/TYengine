#include "GameCore.h"
#include "ParticleClass.h"
#include "Audio/Audio.h"
#include "SceneFactory.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void GameCore::Initialize()
{
	TYFrameWork::Initialize();

	imgui = ImGuiManager::GetInstance();
	imgui->Initialize(windowsApp.get(), directXBasis.get());

	spriteBasis = SpriteBasis::GetInstance();
	spriteBasis->Initialize(directXBasis.get());

	camera = std::make_unique<Camera>();

	object3dBasis = Object3dBasis::GetInstance();
	object3dBasis->Initialize(directXBasis.get());
	object3dBasis->SetDefaultCamera(camera.get());

	modelManager = ModelManager::GetInstance();
	modelManager->Initialize(directXBasis.get(), srvManager.get());

	Audio::GetInstance()->Initialize();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("TITLE");

	ParticleClass::GetInstance()->Initialize(directXBasis.get(), srvManager.get(), camera.get());


	renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(directXBasis.get(), srvManager.get(), 1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, { 1,0,0,1 });

	copyPass = std::make_unique<CopyPass>();
	copyPass->Initialize(directXBasis.get(), srvManager.get(), L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/CopyImage.PS.hlsl");
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

	renderTexture->EndRender();

	// ---------- SwapChainへの描画 ----------
	directXBasis->DrawBegin();

	copyPass->Draw(directXBasis->GetCommandList(), renderTexture->GetGPUHandle()); 

	// ImGuiはSwapChainに描く（上書き）
	imgui->Draw();

	directXBasis->DrawEnd();
}
