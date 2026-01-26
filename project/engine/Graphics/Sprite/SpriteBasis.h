#pragma once
#include "DirectXBasis.h"
#include "SingletonObject.h"

/// <summary>
/// スプライト描画の共通設定（ルートシグネチャ／グラフィックスパイプライン等）を一元管理するクラス。  
/// シングルトンとして提供し、スプライト描画時の共通セットアップを行う。
/// </summary>
class SpriteBasis :
    public SingletonObject<SpriteBasis>
{
    friend class SingletonObject<SpriteBasis>;
    friend struct std::default_delete<SpriteBasis>;

private:
    // 外部からの new/delete を禁止
    SpriteBasis() = default;
    ~SpriteBasis() = default;

public:

    /// <summary>
    /// 初期化処理。DirectX 基盤を登録し、必要なリソース/状態を構築する。
    /// </summary>
    /// <param name="directXBasis">DirectX 基盤（デバイス・コマンド関連）。</param>
    void Initialize(DirectXBasis* directXBasis);

    /// <summary>
    /// スプライト描画の共通設定をコマンドリストへ反映する。  
    /// ルートシグネチャ／PSO／ブレンド／ラスタライザ等の状態をセット。
    /// </summary>
    void BasisDrawSetting();

    /// <summary>登録済みの DirectX 基盤を取得する。</summary>
    /// <returns>DirectXBasis へのポインタ。</returns>
    DirectXBasis* GetDirectXBasis() const { return directXBasis_; }

    /// <summary>スプライト用ルートシグネチャを取得する。</summary>
    /// <returns>ID3D12RootSignature へのポインタ。</returns>
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }

    /// <summary>スプライト用グラフィックスパイプラインステートを取得する。</summary>
    /// <returns>ID3D12PipelineState へのポインタ。</returns>
    ID3D12PipelineState* GetGraphicsPipelineState() const { return graphicsPipelineState_.Get(); }

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
    /// <summary>DirectX 基盤への参照。</summary>
    DirectXBasis* directXBasis_ = nullptr;

    /// <summary>スプライト描画用ルートシグネチャ。</summary>
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    /// <summary>スプライト描画用パイプラインステート（PSO）。</summary>
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

    /// <summary>ブレンドステート記述子。</summary>
    D3D12_BLEND_DESC blendDesc_{};

    /// <summary>ラスタライザステート記述子。</summary>
    D3D12_RASTERIZER_DESC rasterizerDesc_{};

    /// <summary>頂点シェーダバイナリ。</summary>
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;

    /// <summary>ピクセルシェーダバイナリ。</summary>
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

    /// <summary>ルートシグネチャ生成時の出力バイナリ。</summary>
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;

    /// <summary>ルートシグネチャ生成時のエラーメッセージ出力。</summary>
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
};
