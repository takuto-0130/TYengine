#include "TYFrameWork.h"

void TYFrameWork::run()
{
	Initialize();
	//メインループ
	while (true)
	{
		// 毎フレーム更新処理
		Update();
		// 終了リクエストでループを抜ける
		if (IsEndRequest()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}

void TYFrameWork::Initialize()
{
#pragma region // 基盤システム初期化

	// ウィンドウズアプリケーション
	windowsApp = std::make_unique<WindowsApp>();
	windowsApp->Initialize();

	directXBasis = DirectXBasis::GetInstance();
	directXBasis->Initialize(windowsApp.get());

	srvManager = std::make_unique<SrvManager>();
	srvManager->Initialize(directXBasis);

	input = Input::GetInstance();
	input->Initialize(windowsApp->GetHwnd());

	TextureManager::GetInstance()->Initialize(directXBasis, srvManager.get());

	sceneManager_ = SceneManager::GetInstance();
#pragma endregion
}

void TYFrameWork::Finalize()
{
	imgui->Finalize();
	input->Finalize();
	windowsApp->Finalize();
}

void TYFrameWork::Update()
{
	input->Update();
	sceneManager_->Update();
}
