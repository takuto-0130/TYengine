#pragma once
#include "WindowsApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include <dxcapi.h>
#include <chrono>
#include "Logger.h"
#include "StringUtility.h"
#include "DirectXTex/DirectXTex.h"
#include "DirectXTex/d3dx12.h"

/// <summary>
/// DirectX 12 の初期化・リソース作成・描画制御をまとめた基盤クラス（シングルトン）。  
/// デバイス／コマンド／スワップチェーン／各ディスクリプタヒープや、  
/// シェーダコンパイル・テクスチャロードなどの共通ユーティリティを提供する。
/// </summary>
class DirectXBasis
{
public: // メンバ関数
    /// <summary>シングルトンインスタンスを取得する。</summary>
    static DirectXBasis* GetInstance()
    {
        static DirectXBasis instance;
        return &instance;
    }

    /// <summary>
    /// 初期化処理。  
    /// デバイス、コマンド、スワップチェーン、各種ヒープ、ImGui、固定FPS 等を構築する。
    /// </summary>
    /// <param name="windowsApp">ウィンドウ管理クラス。</param>
    void Initialize(WindowsApp* windowsApp);

    /// <summary>指定インデックスの SRV CPU ディスクリプタハンドルを取得する。</summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCpuDescriptorHandle(const uint32_t& index);

    /// <summary>指定インデックスの SRV GPU ディスクリプタハンドルを取得する。</summary>
    D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGpuDescriptorHandle(const uint32_t& index);

    /// <summary>
    /// HLSL シェーダをコンパイルする。  
    /// 例：vs_6_7, ps_6_7 等のプロファイルを指定。
    /// </summary>
    /// <param name="filePath">シェーダファイルパス（.hlsl）。</param>
    /// <param name="profile">ターゲットプロファイル。</param>
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

    /// <summary>任意サイズのバッファリソース（UPLOAD）を作成する。</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const size_t& sizeInBytes);

    /// <summary>メタデータに基づきテクスチャリソースを作成する（DEFAULT）。</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

    /// <summary>生成済みテクスチャへミップ画像群をアップロードする。</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

    /// <summary>ファイルからテクスチャを読み込む（WIC/DirectXTex）。</summary>
    DirectX::ScratchImage LoadTexture(const std::string& filePath);

    /// <summary>深度ステンシルビューをクリアする。</summary>
    void ClearDepthStencilView();

    /// <summary>フレームの描画前処理（バックバッファ遷移、RTV/DSV 設定等）。</summary>
    void DrawBegin();

    /// <summary>フレームの描画後処理（バックバッファ遷移、Present 等）。</summary>
    void DrawEnd();

    /// <summary>コマンドリストのクローズ・実行・フェンス待機を行う。</summary>
    void CommandListAndFence();

    /// <summary>デバイスを取得する。</summary>
    ID3D12Device* GetDevice() const { return device_.Get(); }

    /// <summary>描画コマンドリストを取得する。</summary>
    ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

    /// <summary>描画コマンドキューを取得する。</summary>
    ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); }

    /// <summary>描画コマンドアロケータを取得する。</summary>
    ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator_.Get(); }

    /// <summary>現在のバックバッファの RTV ハンドルを取得する。</summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferRTV() const { return rtvHandles_[swapChain_->GetCurrentBackBufferIndex()]; }

    /// <summary>現在のビューポート設定を取得する。</summary>
    D3D12_VIEWPORT GetViewport() const { return viewportRect_; }

    /// <summary>現在のシザー矩形を取得する。</summary>
    D3D12_RECT GetScissorRect() const { return scissorRect_; }

private: // メンバ関数
    /// <summary>DXGI ファクトリと D3D12 デバイスを初期化する。</summary>
    void InitDevice();

    /// <summary>コマンドアロケータ／リスト／キューを初期化する。</summary>
    void InitCommand();

    /// <summary>スワップチェーンを生成する。</summary>
    void CreateSwapChain();

    /// <summary>深度バッファを生成する。</summary>
    void CreateDepthBuffer();

    /// <summary>RTV/SRV/DSV など各種ディスクリプタヒープを生成する。</summary>
    void CreateVariousDescriptorHeap();

    /// <summary>バックバッファ用 RTV を初期化する。</summary>
    void InitRTV();

    /// <summary>DSV を初期化する。</summary>
    void InitDSV();

    /// <summary>フェンスを初期化する。</summary>
    void InitFence();

    /// <summary>ビューポート矩形を初期化する。</summary>
    void InitViewportRect();

    /// <summary>シザー矩形を初期化する。</summary>
    void InitScissorRect();

    /// <summary>DXC コンパイラ（Utils / Compiler / IncludeHandler）を生成する。</summary>
    void CreateDXCCompiler();

    /// <summary>ImGui を初期化する。</summary>
    void InitImGui();

    /// <summary>固定 FPS の初期化を行う。</summary>
    void InitFixFPS();

    /// <summary>固定 FPS の更新（スリープ制御）。</summary>
    void UpdateFixFPS();

public:
    /// <summary>
    /// 任意タイプのディスクリプタヒープを生成する。  
    /// shaderVisible = true で GPU 可視ヒープ（CBV/SRV/UAV 等）。
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE& heapType, const UINT& numDescriptors, const bool& shaderVisible);

    /// <summary>サンプラ用ディスクリプタヒープを作成する。</summary>
    void CreateSamplerHeap();

    /// <summary>指定インデックスの CPU ハンドルを取得（任意ヒープ）。</summary>
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);

    /// <summary>指定インデックスの GPU ハンドルを取得（任意ヒープ）。</summary>
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, const uint32_t& descriptorSize, const uint32_t& index);

    /// <summary>バックバッファ枚数を取得する。</summary>
    size_t GetBackBufferCount() { return backBuffers_.size(); }

    /// <summary>RTV のディスクリプタサイズを取得する。</summary>
    uint32_t GetDescriptorSizeRTV() const { return descriptorSizeRTV_; }

    /// <summary>DSV のディスクリプタサイズを取得する。</summary>
    uint32_t GetDescriptorSizeDSV() const { return descriptorSizeDSV_; }

    /// <summary>サンプラヒープを取得する。</summary>
    ID3D12DescriptorHeap* GetSamplerHeap() const { return samplerHeap_.Get(); }

    /// <summary>サンプラヒープの GPU 先頭ハンドルを取得する。</summary>
    D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerDescriptorHandle() const { return samplerHeap_->GetGPUDescriptorHandleForHeapStart(); }

    /// <summary>DSV の CPU ハンドルを取得する。</summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHandle_; }

    /// <summary>RTV の CPU ハンドルをインデックス指定で取得する。</summary>
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t index) const { return rtvHandles_[index]; }

    /// <summary>スワップチェーンを取得する。</summary>
    IDXGISwapChain4* GetSwapChain() const { return swapChain_.Get(); }

public: // メンバ変数
    /// <summary>最大 SRV 数（最大テクスチャ数）。</summary>
    static const uint32_t kMaxSRVCount_;

private: // メンバ変数
    WindowsApp* windowsApp_ = nullptr; ///< ウィンドウ管理。

    // Direct3D / DXGI 関連
    Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
    Microsoft::WRL::ComPtr<ID3D12Device> device_;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
    Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescripterHeap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> samplerHeap_;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> backBuffers_;
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2] = {};
    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_ = {};

    Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
    uint32_t fenceValue_ = 0;
    HANDLE fenceEvent_ = {};

    Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
    Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
    Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

    D3D12_RESOURCE_BARRIER barrier_{};

    D3D12_VIEWPORT viewportRect_{};
    D3D12_RECT     scissorRect_{};

    uint32_t descriptorSizeSRV_ = 0;
    uint32_t descriptorSizeRTV_ = 0;
    uint32_t descriptorSizeDSV_ = 0;

    /// <summary>固定 FPS 用の基準時間。</summary>
    std::chrono::steady_clock::time_point reference_;
};
