#pragma once
#include "WindowsApp.h"
#include "DirectXBasis.h"
#include <memory>
#include <mutex>
#include "SingletonObject.h"

/// <summary>
/// ImGuiの管理クラス。
/// 初期化、フレーム開始・終了処理、描画実行を担当する。
/// </summary>
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
	/// <summary>
	/// 初期化処理。
	/// DirectXデバイスやウィンドウハンドルとの紐付けを行う。
	/// </summary>
	/// <param name="winApp">Windowsアプリケーション基盤クラス。</param>
	/// <param name="dxBasis">DirectX基盤クラス。</param>
	void Initialize(WindowsApp* winApp, DirectXBasis* dxBasis);

	/// <summary>
	/// ImGuiのフレーム受付開始。
	/// 毎フレームの描画処理の最初に呼び出す。
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGuiのフレーム受付終了。
	/// 描画コマンドの生成を行う。
	/// </summary>
	void End();

	/// <summary>
	/// 実際の描画コマンドを発行する。
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了処理。リソースの開放など。
	/// </summary>
	void Finalize();
private:
	/// <summary>Windowsアプリケーション基盤クラス。</summary>
	WindowsApp* winApp_ = nullptr;
	/// <summary>DirectX基盤クラス。</summary>
	DirectXBasis* dxBasis_ = nullptr;

	/// <summary>ImGui用SRVディスクリプタヒープ。</summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

