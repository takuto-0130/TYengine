#include "TYFrameWork.h"

void TYFrameWork::run()
{
	// 初期化
	Initialize();
	
	// メインループ
	while (true)
	{
		// 毎フレーム更新処理（入力、シーン更新など）
		Update();
		// 終了リクエストがあればループを抜ける（ウィンドウ閉じるボタン等）
		if (IsEndRequest()) {
			break;
		}
		// 描画処理
		Draw();
	}
	// ゲームの終了処理（リソース解放）
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
