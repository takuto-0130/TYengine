#pragma once
#include <string>
#include <unordered_map>
#include <d3d12.h>
#include <wrl.h>
#include "struct.h"
#include "DirectXTex/DirectXTex.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "SingletonObject.h"

static constexpr size_t FrameCount = 3;

namespace TYEngine {
namespace Graphics {

// using namespace Core;

/// <summary>
/// テクスチャの読み込み・管理を一元化するクラス。  
/// ファイルパスをキーにテクスチャをロード・キャッシュし、SRV を自動生成して扱いやすくする。
/// </summary>
class TextureManager :
    public TYEngine::Utility::SingletonObject<TextureManager>
{
    friend class TYEngine::Utility::SingletonObject<TextureManager>;
    friend struct std::default_delete<TextureManager>;

private:
    // 外部からの new/delete を禁止
    TextureManager() = default;
    ~TextureManager() = default;

public:
    /// <summary>
    /// 初期化処理。DirectX 基盤および SRV 管理クラスを登録する。
    /// </summary>
    /// <param name="dxBasis">DirectX 基盤。</param>
    /// <param name="srvManager">SRV 管理クラス。</param>
    void Initialize(TYEngine::Core::DirectXBasis* dxBasis, SrvManager* srvManager);

    /// <summary>
    /// テクスチャをファイルから読み込み、SRV を作成して管理対象に追加する。  
    /// 同じファイルを複数回読み込んでもキャッシュされたものを再利用する。
    /// </summary>
    /// <param name="filePath">読み込むテクスチャファイルのパス。</param>
    void LoadTexture(const std::string& filePath);

    /// <summary>
    /// ファイルパスから対応するテクスチャの SRV インデックスを取得する。
    /// </summary>
    /// <param name="filePath">検索対象のファイルパス。</param>
    /// <returns>SRV インデックス。</returns>
    uint32_t GetTextureIndexByFilePath(const std::string& filePath);

    /// <summary>
    /// ファイルパスから GPU SRV ハンドルを取得する。
    /// </summary>
    /// <param name="filePath">検索対象のファイルパス。</param>
    /// <returns>対応する GPU SRV ハンドル。</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

    /// <summary>
    /// 内部で使用している SRV 管理クラスを取得する。
    /// </summary>
    SrvManager* GetSrvManager() const { return srvManager_; }

    /// <summary>
    /// 指定テクスチャのメタデータ情報を取得する（幅・高さ・フォーマットなど）。
    /// </summary>
    /// <param name="filePath">対象テクスチャのパス。</param>
    /// <returns>テクスチャのメタデータ構造体。</returns>
    const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

    /// <summary>
    /// フレーム終了時に呼び出す。アップロードバッファの寿命を管理し、次フレームに備える。
    /// </summary>
    void NextFrame();

    /// <summary>
    /// ダミーのキューブマップ（空の環境マップなど）用の GPU SRV ハンドルを取得する。
    /// </summary>
    /// <returns>ダミーキューブマップの GPU SRV ハンドル。</returns>
    D3D12_GPU_DESCRIPTOR_HANDLE GetDummyCubemapHandleGPU() const
    {
        return srvManager_->GetGPUDescriptorHandle(dummyCubemapIndex_);
    }

private:
    /// <summary>
    /// ダミーキューブマップ（全ピクセル黒）を生成し、SRV を登録する。
    /// </summary>
    void CreateDummyCubemap();

private:
    /// <summary>
    /// テクスチャ情報構造体。  
    /// メタデータ・リソース・ディスクリプタ情報を保持する。
    /// </summary>
    struct TextureData
    {
        DirectX::TexMetadata metadata;                    ///< テクスチャのメタデータ。
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;  ///< テクスチャリソース。
        uint32_t srvIndex = 0;                            ///< SRV のインデックス。
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};       ///< CPU SRV ハンドル。
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};       ///< GPU SRV ハンドル。
    };
    TYEngine::Core::DirectXBasis* dxBasis_ = nullptr;   ///< DirectX 基盤。
    SrvManager* srvManager_ = nullptr;  ///< SRV 管理クラス。

    std::unordered_map<std::string, TextureData> textureData_; ///< ファイルパスをキーにしたテクスチャデータ管理。

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> temporaryBuffers_; ///< 一時アップロード用バッファ。

    size_t currentFrameIndex_ = 0; ///< 現在のフレームインデックス。
    std::array<std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>, FrameCount> frameUploadBuffers_; ///< フレームごとのアップロードバッファ。

    Microsoft::WRL::ComPtr<ID3D12Resource> dummyCubemap_;              ///< ダミーキューブマップリソース。
    uint32_t dummyCubemapIndex_ = 0;                                   ///< ダミーキューブマップの SRV インデックス。
    Microsoft::WRL::ComPtr<ID3D12Resource> dummyCubemapUploadBuffer_;  ///< ダミー用アップロードバッファ。
};

} // namespace Graphics
} // namespace TYEngine
