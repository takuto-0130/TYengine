#pragma once
#include "Camera.h"
#include "DirectXBasis.h"

/// <summary>
/// キューブマップ（スカイボックス）描画の基盤クラス。  
/// DirectX のルートシグネチャ・パイプライン・カメラを管理し、  
/// スカイボックスの描画を簡単に実行できるようにするシングルトンクラス。
/// </summary>
class CubemapBasis
{
public:
    /// <summary>
    /// シングルトンインスタンスを取得する。
    /// </summary>
    /// <returns>CubemapBasis の唯一のインスタンス。</returns>
    static CubemapBasis* GetInstance()
    {
        static CubemapBasis instance; ///< 静的ローカル変数として一度だけ生成される。
        return &instance;
    }

    /// <summary>デストラクタ。</summary>
    ~CubemapBasis() = default;

private:
    /// <summary>コンストラクタ（外部からの生成を禁止）。</summary>
    CubemapBasis() = default;

    /// <summary>コピーコンストラクタ（禁止）。</summary>
    CubemapBasis(CubemapBasis&) = delete;

    /// <summary>代入演算子（禁止）。</summary>
    CubemapBasis& operator=(CubemapBasis&) = delete;

public:
    /// <summary>
    /// 初期化処理。  
    /// DirectX 基盤を登録し、キューブマップ描画に必要なリソースを生成する。
    /// </summary>
    /// <param name="dxBasis">DirectX の基盤クラス。</param>
    void Initialize(DirectXBasis* dxBasis);

    /// <summary>
    /// キューブマップ描画の前処理を行う。  
    /// パイプラインやルートシグネチャをセットアップして描画準備を行う。
    /// </summary>
    void DrawBegin();

    /// <summary>
    /// 使用中のルートシグネチャを取得する。
    /// </summary>
    /// <returns>ID3D12RootSignature ポインタ。</returns>
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

    /// <summary>
    /// 使用中のパイプラインステートを取得する。
    /// </summary>
    /// <returns>ID3D12PipelineState ポインタ。</returns>
    ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState_.Get(); }

    /// <summary>
    /// デフォルトカメラを設定する。
    /// </summary>
    /// <param name="camera">使用するカメラへのポインタ。</param>
    void SetDefaultCamera(Camera* camera) { defaultCamera_ = camera; }

    /// <summary>
    /// 現在設定されているデフォルトカメラを取得する。
    /// </summary>
    /// <returns>カメラへのポインタ。</returns>
    Camera* GetDefaultCamera() const { return defaultCamera_; }

    /// <summary>
    /// 登録されている DirectX 基盤を取得する。
    /// </summary>
    /// <returns>DirectXBasis ポインタ。</returns>
    DirectXBasis* GetDirectXBasis() const { return directXBasis_; }

private: // 内部関数
    /// <summary>
    /// ルートシグネチャを作成する。  
    /// シェーダリソースや定数バッファのバインド設定を構築する。
    /// </summary>
    void CreateRootSignature();

    /// <summary>
    /// グラフィックスパイプラインを作成する。  
    /// スカイボックス用のシェーダ、ブレンド、ラスタライザ設定を行う。
    /// </summary>
    void CreateGraphicsPipeline();

private: // メンバ変数
    DirectXBasis* directXBasis_ = nullptr; ///< DirectX の基盤。
    Camera* defaultCamera_ = nullptr;      ///< デフォルトカメラ。

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;       ///< ルートシグネチャ。
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_; ///< パイプラインステート。

    D3D12_BLEND_DESC blendDesc_{};       ///< ブレンド設定。
    D3D12_RASTERIZER_DESC rasterizerDesc_{}; ///< ラスタライザ設定。

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr; ///< 頂点シェーダ。
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;  ///< ピクセルシェーダ。

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr; ///< シグネチャデータ。
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;     ///< エラーメッセージ格納用。
};
