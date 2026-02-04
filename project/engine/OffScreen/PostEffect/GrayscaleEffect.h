#pragma once
#include "PostEffectBase.h"

/// <summary>
/// グレースケール（白黒化）ポストエフェクト。  
/// シーン全体の彩度を下げ、モノクロ風の表現を行う。  
/// フィルム調の演出や被ダメージ演出などにも応用可能。
/// </summary>
namespace TYEngine {
namespace OffScreen {

using namespace Core; // For DirectXBasis if needed

/// <summary>
/// グレースケール（白黒化）ポストエフェクト。  
/// シーン全体の彩度を下げ、モノクロ風の表現を行う。  
/// フィルム調の演出や被ダメージ演出などにも応用可能。
/// </summary>
class GrayscaleEffect
    : public PostEffectBase
{
public:
    /// <summary>
    /// 初期化処理。  
    /// DirectX リソースおよび SRV 管理をセットアップする。
    /// </summary>
    /// <param name="dx">DirectX 基盤。</param>
    /// <param name="srv">SRV 管理クラス。</param>
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    /// <summary>
    /// 毎フレームの更新処理。  
    /// 内部パラメータや補間値を更新する。
    /// </summary>
    void Update() override;

    /// <summary>
    /// ImGui によるデバッグ UI 表示。  
    /// 強度や色味のパラメータをリアルタイムで調整可能。
    /// </summary>
    void ImGuiUpdate() override;

    /// <summary>
    /// グレースケール効果を入力テクスチャに適用する。
    /// </summary>
    /// <param name="input">処理対象のレンダーターゲット。</param>
    void Apply(RenderTexture* input) override;

public:
    /// <summary>
    /// グレースケールの強度を設定する。  
    /// 0.0f で無効、1.0f で完全な白黒。
    /// </summary>
    /// <param name="s">強度値。</param>
    void SetStrength(float s) { param_->strength = s; }

    /// <summary>
    /// グレースケール時のティントカラー（色味）を設定する。  
    /// わずかに色味を残したい場合に使用。
    /// </summary>
    /// <param name="color">ティントカラー。</param>
    void SetTintColor(const Vector3& color) { param_->tintColor = color; }

    /// <summary>グレースケールの強度を取得する。</summary>
    float GetStrength() const { return param_->strength; }

    /// <summary>現在設定されているティントカラーを取得する。</summary>
    Vector3 GetTintColor() const { return param_->tintColor; }

private:
    /// <summary>
    /// グレースケールエフェクト用のパラメータ構造体。  
    /// 強度と色味の制御を行う。
    /// </summary>
    struct GrayscaleParam
    {
        float strength = 1.0f;                   ///< グレースケール強度（0.0～1.0）
        Vector3 tintColor = { 1.0f, 1.0f, 1.0f }; ///< ティントカラー（色味補正用）
    };

    /// <summary>エフェクトパラメータ。</summary>
    std::shared_ptr<GrayscaleParam> param_;
};

} // namespace OffScreen
} // namespace TYEngine
