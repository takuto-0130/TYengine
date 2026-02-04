#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <memory>
#include "DirectXBasis.h"
#include "struct.h"

class SrvManager;

namespace TYEngine {
namespace OffScreen {

using namespace Core; // For DirectXBasis

/// <summary>
/// コピー用の定数バッファパラメータ。
/// オフセットとスケールを指定することで、描画範囲を調整できる。
/// </summary>
struct CopyPassParam
{
    Vector2 offset; ///< UV オフセット。
    Vector2 scale;  ///< UV スケール。
};

/// <summary>
/// 追加の定数バッファ情報。  
/// 登録番号（registerIndex）と対応する GPU リソースを保持する。
/// </summary>
struct ExtraBuffer
{
    UINT registerIndex; ///< ルートパラメータの登録インデックス。
    Microsoft::WRL::ComPtr<ID3D12Resource> resource; ///< バッファリソース。
    void* mappedPtr; ///< CPU 側のマッピングアドレス。
};

/// <summary>
/// コピー描画を行うポストエフェクトパス。  
/// 主にフルスクリーン描画やマスク合成、エフェクト適用などに使用する。
/// </summary>
class CopyPass
{
public:
    /// <summary>
    /// 初期化処理。  
    /// DirectX 基盤、SRV マネージャ、シェーダファイルを指定してコピー用パイプラインを構築する。
    /// </summary>
    /// <param name="dxBasis">DirectX 基盤クラス（デバイス・コマンド関連）</param>
    /// <param name="srvMgr">SRV 管理クラス</param>
    /// <param name="vsPath">頂点シェーダのパス</param>
    /// <param name="psPath">ピクセルシェーダのパス</param>
    void Initialize(DirectXBasis* dxBasis, SrvManager* srvMgr, const std::wstring& vsPath, const std::wstring& psPath);

    /// <summary>
    /// パラメータの更新処理。  
    /// 毎フレーム呼び出して、オフセットやスケールなどを GPU に反映する。
    /// </summary>
    void Update();

    /// <summary>
    /// コピー描画を実行する。  
    /// SRV を使用してフルスクリーン描画を行う。
    /// </summary>
    /// <param name="cmdList">描画コマンドリスト</param>
    /// <param name="srvHandle">描画対象の SRV ハンドル</param>
    /// <param name="toSwapChain">スワップチェーンへ直接出力する場合は true</param>
    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, bool toSwapChain = false);

    /// <summary>
    /// 個別の定数バッファを追加する。  
    /// 任意の型の構造体を追加し、シェーダ側で別の register 番号として扱える。
    /// </summary>
    /// <typeparam name="T">追加するバッファの型</typeparam>
    /// <param name="registerIndex">ルートパラメーターのインデックス（例：4, 5）</param>
    /// <param name="defaultValue">初期値</param>
    /// <returns>mappedPtr を shared_ptr として外部に渡す（resource の寿命に従う）</returns>
    template<typename T>
    std::shared_ptr<T> AddExtraConstantBuffer(UINT registerIndex, const T& defaultValue = {})
    {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = dxBasis_->CreateBufferResource(sizeof(T));
        void* mapped = nullptr;
        CD3DX12_RANGE readRange(0, 0);
        resource->Map(0, &readRange, &mapped);
        memcpy(mapped, &defaultValue, sizeof(T));

        extraBuffers_.push_back({ registerIndex, resource, mapped });

        // shared_ptr で mappedPtr を外部に渡す（resource の寿命に従う）
        return std::shared_ptr<T>(reinterpret_cast<T*>(mapped), [resource](T*) {});
    }

    /// <summary>
    /// 指定したパスのマスクテクスチャを読み込み、SRV として設定する。
    /// </summary>
    /// <param name="filePath">読み込むテクスチャファイルのパス</param>
    void LoadAndSetMaskTexture(const std::string& filePath);

    /// <summary>
    /// 深度 SRV を設定する。  
    /// DoF や被写界深度エフェクトなど、深度情報を使用する場合に呼び出す。
    /// </summary>
    /// <param name="depthSrv">深度テクスチャの SRV ハンドル</param>
    void SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE depthSrv)
    {
        extraSrvHandle_ = depthSrv;
        useExtraTexture_ = true;
    }

private:
    /// <summary>DirectX 基盤クラスへの参照</summary>
    DirectXBasis* dxBasis_ = nullptr;

    /// <summary>SRV 管理クラスへの参照</summary>
    SrvManager* srvMgr_ = nullptr;

    /// <summary>ルートシグネチャ</summary>
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    /// <summary>パイプラインステートオブジェクト（PSO）</summary>
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    /// <summary>コピー用定数バッファリソース</summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> copyParamBuffer_;

    /// <summary>コピー用パラメータ（CPU側から書き込み用）</summary>
    CopyPassParam* mappedParam_ = nullptr;

    /// <summary>追加定数バッファ群（ユーザー拡張用）</summary>
    std::vector<ExtraBuffer> extraBuffers_;

    /// <summary>追加の SRV ハンドル（深度やマスク用）</summary>
    D3D12_GPU_DESCRIPTOR_HANDLE extraSrvHandle_{};

    /// <summary>追加 SRV を使用するかどうかのフラグ</summary>
    bool useExtraTexture_ = false;
};

} // namespace OffScreen
} // namespace TYEngine


