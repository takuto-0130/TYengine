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
	windowsApp_ = std::make_unique<WindowsApp>();
	windowsApp_->Initialize();

	directXBasis_ = DirectXBasis::GetInstance();
	directXBasis_->Initialize(windowsApp_.get());

	srvManager_ = std::make_unique<SrvManager>();
	srvManager_->Initialize(directXBasis_);

	input_ = Input::GetInstance();
	input_->Initialize(windowsApp_->GetHwnd());

	TextureManager::GetInstance()->Initialize(directXBasis_, srvManager_.get());

	sceneManager_ = SceneManager::GetInstance();
#pragma endregion
}

void TYFrameWork::Finalize()
{
	imgui_->Finalize();
	windowsApp_->Finalize();
}

void TYFrameWork::Update()
{
	input_->Update();
	sceneManager_->Update();
}
