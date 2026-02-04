#pragma once
#include "PostEffectBase.h"

/// <summary>
/// ボックスフィルタ（平均化フィルタ）を適用するポストエフェクトクラス。  
/// 指定されたカーネル範囲内の画素の平均を取り、全体的に柔らかくぼかす効果を与える。
/// </summary>
namespace TYEngine {
namespace OffScreen {

using namespace Core; // For WindowsApp usually, but check usage. 
// BoxFilterParam uses WindowsApp::kClientWidth. WindowsApp is in TYEngine::Core.

/// <summary>
/// ボックスフィルタ（平均化フィルタ）を適用するポストエフェクトクラス。  
/// 指定されたカーネル範囲内の画素の平均を取り、全体的に柔らかくぼかす効果を与える。
/// </summary>
class BoxFilterEffect :
    public PostEffectBase
{
public:
    /// <summary>
    /// 初期化処理。  
    /// シェーダ、定数バッファ、SRV のセットアップを行う。
    /// </summary>
    /// <param name="dx">DirectX 基盤。</param>
    /// <param name="srv">SRV 管理クラス。</param>
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    /// <summary>
    /// 毎フレームの更新処理。  
    /// 内部パラメータの補間や時間変化を反映する。
    /// </summary>
    void Update() override;

    /// <summary>
    /// ImGui によるデバッグ UI 表示。  
    /// カーネルサイズやテクセルサイズのリアルタイム調整が可能。
    /// </summary>
    void ImGuiUpdate() override;

    /// <summary>
    /// 入力テクスチャにボックスフィルタを適用し、平均化ぼかし処理を行う。
    /// </summary>
    /// <param name="input">処理対象のレンダーターゲット。</param>
    void Apply(RenderTexture* input) override;

public:
    // ========================
    //        Setter
    // ========================

    /// <summary>
    /// フィルタのカーネルサイズを設定する。  
    /// 値が大きいほど広範囲にぼかしがかかる。
    /// </summary>
    /// <param name="size">カーネルサイズ。</param>
    void SetKernelSize(int size) { param_->kernelSize = size; }

    /// <summary>
    /// テクセルサイズを設定する。  
    /// 通常は描画ターゲットの解像度に応じて設定される。
    /// </summary>
    /// <param name="texel">テクセルサイズ（1 / width, 1 / height）。</param>
    void SetTexelSize(const Vector2& texel) { param_->texelSize = texel; }

    // ========================
    //        Getter
    // ========================

    /// <summary>現在のカーネルサイズを取得する。</summary>
    int GetKernelSize() const { return param_->kernelSize; }

    /// <summary>現在のテクセルサイズを取得する。</summary>
    Vector2 GetTexelSize() const { return param_->texelSize; }

private:
    /// <summary>
    /// ボックスフィルタ用のパラメータ構造体。  
    /// カーネルサイズとテクセルサイズを保持し、GPU 側で平均化処理に使用される。
    /// </summary>
    struct BoxFilterParam
    {
        int kernelSize = 5; ///< フィルタカーネルのサイズ。
        Vector2 texelSize = { 1.0f / WindowsApp::kClientWidth, 1.0f / WindowsApp::kClientHeight }; ///< 1ピクセルあたりのUV単位（解像度に依存）。
    };

    /// <summary>ボックスフィルタのパラメータ。</summary>
    std::shared_ptr<BoxFilterParam> param_;
};

} // namespace OffScreen
} // namespace TYEngine
