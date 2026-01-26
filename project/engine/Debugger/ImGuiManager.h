#pragma once
#include "WindowsApp.h"
#include "DirectXBasis.h"
#include <memory>
#include <mutex>
#include "SingletonObject.h"

// ImGui管理クラス
class ImGuiManager :
	public SingletonObject<ImGuiManager>
{
	friend class SingletonObject<ImGuiManager>;
	friend struct std::default_delete<ImGuiManager>;

private:
	// 外部からの new/delete を禁止
	ImGuiManager() = default;
	~ImGuiManager() = default;

public:
	// 初期化
	void Initialize(WindowsApp* winApp, DirectXBasis* dxBasis);
	// ImGui受付開始
	void Begin();
	// ImGui受付終了
	void End();
	// 描画
	void Draw();
	// 終了
	void Finalize();
private:
	WindowsApp* winApp_ = nullptr;
	DirectXBasis* dxBasis_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

