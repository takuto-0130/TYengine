#pragma once
#include "PostEffectBase.h"

/// <summary>
/// ランダムノイズ（フィルムグレイン）効果を適用するポストエフェクトクラス。  
/// 画面全体に粒状のノイズを重ねることで、映像の質感を演出する。
/// </summary>
class RandomEffect :
    public PostEffectBase
{
public:
    /// <summary>
    /// エフェクトの初期化を行う。  
    /// シェーダー・定数バッファ・パラメータの準備を行う。
    /// </summary>
    /// <param name="dx">DirectX基盤クラス（デバイス・コマンド管理）</param>
    /// <param name="srv">SRV管理クラス</param>
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    /// <summary>
    /// 毎フレームの更新処理を行う。  
    /// 時間経過に応じてノイズパターンを変化させるなどの処理を実装可能。
    /// </summary>
    void Update() override;

    /// <summary>
    /// ImGuiによるパラメータ編集UIを表示する。  
    /// ノイズ強度や色味の調整を行える。
    /// </summary>
    void ImGuiUpdate() override;

    /// <summary>
    /// 入力テクスチャにノイズ効果を適用し、描画結果を出力する。
    /// </summary>
    /// <param name="input">入力となるレンダーターゲット</param>
    void Apply(RenderTexture* input) override;

public:
    /// <summary>ノイズ更新用の時間パラメータを設定する</summary>
    /// <param name="t">時間またはランダムシード値</param>
    void SetT(float t) { param_->t = t; }

    /// <summary>ノイズの強度を設定する</summary>
    /// <param name="s">強度値（0で無効、1以上で強いノイズ）</param>
    void SetNoiseStrength(float s) { param_->noiseStrength = s; }

    /// <summary>ノイズの色味を設定する</summary>
    /// <param name="c">ノイズカラー（RGB）</param>
    void SetNoiseTint(const Vector3& c) { param_->noiseTint = c; }

    /// <summary>現在の時間（t）パラメータを取得する</summary>
    float GetT() const { return param_->t; }

    /// <summary>現在のノイズ強度を取得する</summary>
    float GetNoiseStrength() const { return param_->noiseStrength; }

    /// <summary>現在のノイズカラーを取得する</summary>
    Vector3 GetNoiseTint() const { return param_->noiseTint; }

private:
    /// <summary>
    /// ランダムノイズ効果用の定数バッファ構造体。  
    /// t：ノイズの時間またはシード  
    /// noiseStrength：ノイズの強度  
    /// noiseTint：ノイズの色味
    /// </summary>
    struct RandomParam
    {
        float t = 1.0f;                     ///< ノイズ更新用の時間またはシード
        float noiseStrength = 0.5f;         ///< ノイズの強度
        float padding[2];                   ///< アライメント用
        Vector3 noiseTint = { 0.3f, 0.3f, 0.3f }; ///< ノイズの色味（RGB）
    };

    /// <summary>ノイズ用のパラメータを保持する共有ポインタ</summary>
    std::shared_ptr<RandomParam> param_;
};


