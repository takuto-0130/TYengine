#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <cstdint>
#include <unordered_map>

namespace TYEngine {
namespace Core { class DirectXBasis; } // Forward declaration with namespace

namespace Graphics {

// using namespace Core;

/// <summary>
/// SRV（Shader Resource View）を一括管理するクラス。  
/// CBV/SRV/UAV ディスクリプタヒープ上の SRV を確保・作成・参照・ルート設定までを提供する。
/// </summary>
class SrvManager
{
public:
    /// <summary>
    /// 初期化処理。SRV 用ディスクリプタヒープを作成する。
    /// </summary>
    /// <param name="dxBasis">DirectX 基盤（デバイス取得に使用）。</param>
    /// <param name="maxDescriptors">確保するディスクリプタ数（既定：256）。</param>
    void Initialize(TYEngine::Core::DirectXBasis* dxBasis, uint32_t maxDescriptors = 256);

    /// <summary>
    /// SRV スロットを 1 つ確保し、そのインデックスを返す。
    /// </summary>
    /// <returns>確保した SRV スロットのインデックス。</returns>
    uint32_t Allocate();

    /// <summary>
    /// 追加の SRV スロットを確保できるかを返す。
    /// </summary>
    /// <returns>確保可能なら true、満杯なら false。</returns>
    bool CanAllocate() const;

    /// <summary>
    /// 2D テクスチャ用の SRV を作成する。
    /// </summary>
    /// <param name="index">作成先のディスクリプタインデックス。</param>
    /// <param name="resource">対象リソース（ID3D12Resource）。</param>
    /// <param name="format">フォーマット。</param>
    /// <param name="mipLevels">ミップレベル数。</param>
    void CreateSRVForTexture2D(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels);

    /// <summary>
    /// 構造化バッファ用の SRV を作成する。
    /// </summary>
    /// <param name="index">作成先のディスクリプタインデックス。</param>
    /// <param name="resource">対象リソース。</param>
    /// <param name="elementCount">要素数。</param>
    /// <param name="elementSize">1 要素のサイズ（バイト）。</param>
    void CreateSRVForStructuredBuffer(uint32_t index, ID3D12Resource* resource, UINT elementCount, UINT elementSize);

    /// <summary>
    /// グラフィックスパイプラインのルートパラメータに SRV テーブルをセットする。
    /// </summary>
    /// <param name="cmdList">コマンドリスト。</param>
    /// <param name="rootParamIndex">ルートパラメータのインデックス。</param>
    /// <param name="index">ディスクリプタインデックス（テーブル先頭）。</param>
    void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex, uint32_t index);

    /// <summary>
    /// 指定インデックスの GPU ディスクリプタハンドル（SRV）を取得する。
    /// </summary>
    /// <param name="index">ディスクリプタインデックス。</param>
    /// <returns>GPU ディスクリプタハンドル。</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) const;

    /// <summary>
    /// 指定インデックスの CPU ディスクリプタハンドル（SRV）を取得する。
    /// </summary>
    /// <param name="index">ディスクリプタインデックス。</param>
    /// <returns>CPU ディスクリプタハンドル。</returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) const;

    /// <summary>
    /// 内部の SRV ディスクリプタヒープを取得する。
    /// </summary>
    /// <returns>ID3D12DescriptorHeap（CBV/SRV/UAV）。</returns>
    ID3D12DescriptorHeap* GetHeap() const { return srvHeap_.Get(); }

    /// <summary>
    /// SRV のディスクリプタサイズ（インクリメントサイズ）を取得する。
    /// </summary>
    /// <returns>ディスクリプタサイズ（バイト数ではなく句のサイズ）。</returns>
    uint32_t GetDescriptorSizeSRV() { return descriptorSize_; }

    /// <summary>
    /// GPU ハンドルから対応する CPU ハンドルを逆引きで取得する。  
    /// 同一ヒープ内での相対オフセット計算により求める。
    /// </summary>
    /// <param name="gpuHandle">GPU ディスクリプタハンドル。</param>
    /// <returns>対応する CPU ディスクリプタハンドル。</returns>
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleFromGpu(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) const;

    /// <summary>
    /// キューブテクスチャ（TextureCube）用の SRV を作成する。
    /// </summary>
    /// <param name="index">作成先のディスクリプタインデックス。</param>
    /// <param name="resource">対象リソース。</param>
    /// <param name="format">フォーマット。</param>
    /// <param name="mipLevels">ミップレベル数。</param>
    void CreateSRVForTextureCube(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels);

private:
    TYEngine::Core::DirectXBasis* dxBasis_ = nullptr;                                   ///< DirectX 基盤。
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;              ///< CBV/SRV/UAV ディスクリプタヒープ。
    uint32_t descriptorSize_ = 0;                                       ///< ディスクリプタインクリメントサイズ。
    uint32_t currentIndex_ = 0;                                         ///< 次に割り当てるインデックス。
    uint32_t maxIndex_ = 0;                                             ///< 確保済みスロット上限。
};

} // namespace Graphics
} // namespace TYEngine
