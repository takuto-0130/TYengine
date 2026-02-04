#pragma once
#include "PostEffectBase.h"

/// <summary>
/// 放射状ブラー（ラディアルブラー）効果を適用するポストエフェクトクラス。  
/// 指定した中心点から外側に向かってブラーをかけることで、速度感や集中線効果を演出する。
/// </summary>
namespace TYEngine {
namespace OffScreen {

using namespace Core; // For DirectXBasis

/// <summary>
/// 放射状ブラー（ラディアルブラー）効果を適用するポストエフェクトクラス。  
/// 指定した中心点から外側に向かってブラーをかけることで、速度感や集中線効果を演出する。
/// </summary>
class RadialBlurEffect :
    public PostEffectBase
{
public:
    /// <summary>
    /// エフェクトの初期化を行う。  
    /// シェーダーや定数バッファの準備を行う。
    /// </summary>
    /// <param name="dx">DirectX基盤クラス（デバイス・コマンド管理）</param>
    /// <param name="srv">SRV管理クラス</param>
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    /// <summary>
    /// 毎フレーム更新処理を行う。  
    /// エフェクトパラメータの動的変化などを反映する。
    /// </summary>
    void Update() override;

    /// <summary>
    /// ImGuiによるパラメータ編集UIを表示する。  
    /// ブラー中心や強度、サンプル数をリアルタイムで調整可能。
    /// </summary>
    void ImGuiUpdate() override;

    /// <summary>
    /// 放射ブラーを適用し、結果を出力する。  
    /// 中心点から外方向にぼかす処理を行う。
    /// </summary>
    /// <param name="input">入力となるレンダーターゲット</param>
    void Apply(RenderTexture* input) override;

public:
    /// <summary>ブラーの中心座標を設定する</summary>
    /// <param name="c">中心位置（UV空間：0〜1）</param>
    void SetCenter(const Vector2& c) { param_->kCenter = c; }

    /// <summary>ブラーの広がり（強度）を設定する</summary>
    /// <param name="w">ブラーの幅（値が大きいほど強くぼける）</param>
    void SetBlurWidth(float w) { param_->kBlurWidth = w; }

    /// <summary>ブラー計算のサンプリング回数を設定する</summary>
    /// <param name="n">サンプル数（大きいほど滑らかだが処理が重くなる）</param>
    void SetNumSamples(int n) { param_->kNumSamples = n; }

    /// <summary>現在のブラー中心座標を取得する</summary>
    Vector2 GetCenter() const { return param_->kCenter; }

    /// <summary>現在のブラー幅を取得する</summary>
    float GetBlurWidth() const { return param_->kBlurWidth; }

    /// <summary>現在のサンプル数を取得する</summary>
    int GetNumSamples() const { return param_->kNumSamples; }

private:
    /// <summary>
    /// 放射ブラー用の定数バッファ構造体。  
    /// kCenter：ブラーの中心点（UV座標）  
    /// kBlurWidth：ブラーの広がり  
    /// kNumSamples：サンプル数
    /// </summary>
    struct RadialBlurParam
    {
        Vector2 kCenter = { 0.5f, 0.5f }; ///< ブラーの中心（0.5,0.5 は画面中央）
        float kBlurWidth = 0.03f;          ///< ブラーの広がり（強度）
        int kNumSamples = 5;               ///< サンプリング回数
    };

    /// <summary>放射ブラー用パラメータを保持する共有ポインタ</summary>
    std::shared_ptr<RadialBlurParam> param_;
};

} // namespace OffScreen
} // namespace TYEngine


