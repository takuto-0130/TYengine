#pragma once
#include "Camera.h"
#include "DirectXBasis.h"
#include "SingletonObject.h"

namespace TYEngine {
namespace Graphics {

using namespace Core; // For DirectXBasis
using namespace Core; // For DirectXBasis
// using namespace Camera; // Removed to avoid ambiguity

/// <summary>
/// 3D オブジェクト描画の共通設定（ルートシグネチャ／グラフィックスパイプライン／ライト等）を一元管理するクラス。  
/// シングルトンとして提供し、3D 描画時の共通セットアップやデフォルトカメラ・スカイボックスの管理を行う。
/// </summary>
class Object3dBasis :
    public SingletonObject<Object3dBasis>
{
    friend class SingletonObject<Object3dBasis>;
    friend struct std::default_delete<Object3dBasis>;

private:
    // 外部からの new/delete を禁止
    Object3dBasis() = default;
    ~Object3dBasis() = default;

public:
    /// <summary>
    /// 初期化処理。DirectX 基盤を登録し、必要なリソース／状態を構築する。
    /// </summary>
    /// <param name="directXBasis">DirectX 基盤（デバイス・コマンド関連）。</param>
    void Initialize(DirectXBasis* directXBasis);

    /// <summary>
    /// 3D 描画の共通設定をコマンドリストへ反映する。  
    /// ルートシグネチャ／PSO／ブレンド／ラスタライザ等の状態をセット。
    /// </summary>
    void BasisDrawSetting();

    /// <summary>登録済みの DirectX 基盤を取得する。</summary>
    /// <returns>DirectXBasis へのポインタ。</returns>
    DirectXBasis* GetDirectXBasis() const { return directXBasis_; }

    /// <summary>3D 用ルートシグネチャを取得する。</summary>
    /// <returns>ID3D12RootSignature へのポインタ。</returns>
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

    /// <summary>3D 用グラフィックスパイプラインステートを取得する。</summary>
    /// <returns>ID3D12PipelineState へのポインタ。</returns>
    ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState_.Get(); }

    /// <summary>デフォルトカメラを設定する。</summary>
    /// <param name="camera">デフォルトで使用するカメラ。</param>
    void SetDefaultCamera(TYEngine::Camera::Camera* camera) { defaultCamera_ = camera; }

    /// <summary>デフォルトカメラを取得する。</summary>
    /// <returns>デフォルトカメラ。</returns>
    TYEngine::Camera::Camera* GetDefaultCamera() const { return defaultCamera_; }

    /// <summary>スカイボックス用テクスチャのファイルパスを設定する。</summary>
    /// <param name="textureFilePath">スカイボックスのテクスチャパス。</param>
    void SetSkyboxFilePath(const std::string& textureFilePath) { skyboxTextureFilePath_ = textureFilePath; }

    /// <summary>スカイボックス用テクスチャのファイルパスを取得する。</summary>
    /// <returns>スカイボックスのテクスチャパス。</returns>
    const std::string& GetSkyboxFilePath() { return skyboxTextureFilePath_; }

private: // メンバ関数
    /// <summary>
    /// ルートシグネチャを作成する（SRV／CBV／サンプラ等のレイアウトを定義）。
    /// </summary>
    void CreateRootSignature();

    /// <summary>
    /// グラフィックスパイプライン（PSO）を作成する。  
    /// シェーダ、入力レイアウト、ブレンド、ラスタライザ、深度等の設定を含む。
    /// </summary>
    void CreateGraphicsPipeline();

private: // メンバ変数
    /// <summary>平行光源（ディレクショナルライト）のパラメータ。</summary>
    struct DirectionalLight
    {
        Vector4 color;      ///< 光の色（RGBA）。
        Vector3 direction;  ///< 光の方向（正規化ベクトル推奨）。
        float intensity;    ///< 光の強度。
    };

    DirectXBasis* directXBasis_ = nullptr;                 ///< DirectX 基盤。
    TYEngine::Camera::Camera* defaultCamera_ = nullptr;                      ///< デフォルトカメラ。

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;   ///< 3D 描画用ルートシグネチャ。
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_; ///< 3D 描画用 PSO。

    D3D12_BLEND_DESC blendDesc_{};           ///< ブレンドステート記述子。
    D3D12_RASTERIZER_DESC rasterizerDesc_{}; ///< ラスタライザステート記述子。

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr; ///< 頂点シェーダバイナリ。
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;  ///< ピクセルシェーダバイナリ。

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;     ///< ルートシグネチャ生成時の出力バイナリ。
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;         ///< ルートシグネチャ生成時のエラーメッセージ。

    Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource; ///< ディレクショナルライト用 CB/リソース。
    DirectionalLight* directionalLightData = nullptr;                 ///< ライト定数バッファのマップ先。

    std::string skyboxTextureFilePath_ = ""; ///< スカイボックス用テクスチャパス。
};

} // namespace Graphics
} // namespace TYEngine
