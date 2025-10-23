#pragma once
#include "CopyPass.h"

/// <summary>
/// 被写界深度（Depth of Field, DoF）効果を適用するポストエフェクトパス。  
/// カメラの焦点距離に基づいて、前景・背景のぼかしを表現する。
/// </summary>
class DoFPass
{
public:
    /// <summary>
    /// 被写界深度パスの初期化を行う。  
    /// シェーダ、ルートシグネチャ、定数バッファなどの準備を行う。
    /// </summary>
    /// <param name="dx">DirectX 基盤。</param>
    /// <param name="srvMgr">SRV 管理クラス。</param>
    void Initialize(DirectXBasis* dx, SrvManager* srvMgr);

    /// <summary>
    /// フレーム更新処理を行う。  
    /// パラメータの補間や時間変化の適用を行う。
    /// </summary>
    void Update();

    /// <summary>
    /// ImGui 上でパラメータを調整するためのデバッグ UI を表示する。
    /// </summary>
    void ImGuiUpdate();

    /// <summary>
    /// 被写界深度効果を描画する。  
    /// 深度テクスチャとカラー入力を用いてボケ処理を行う。
    /// </summary>
    /// <param name="cmdList">描画コマンドリスト。</param>
    /// <param name="inputSrv">入力カラーの GPU SRV ハンドル。</param>
    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv);

    /// <summary>
    /// 深度テクスチャ（SRV）を設定する。  
    /// DoF 計算に使用される深度情報を指定する。
    /// </summary>
    /// <param name="handle">深度テクスチャの GPU SRV ハンドル。</param>
    void SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle);

    // ========================
    //        Setter
    // ========================

    /// <summary>フォーカスが合う深度値を設定する。</summary>
    /// <param name="depth">焦点距離（Z深度）。</param>
    void SetFocusDepth(float depth) { dofParam_->focusDepth = depth; }

    /// <summary>ぼかしの強度スケールを設定する。</summary>
    /// <param name="scale">ブラー強度スケール。</param>
    void SetBlurScale(float scale) { dofParam_->blurScale = scale; }

    /// <summary>カメラの近遠クリップ面を設定する。</summary>
    /// <param name="nearZ">Near クリップ面。</param>
    /// <param name="farZ">Far クリップ面。</param>
    void SetNearFar(float nearZ, float farZ) { dofParam_->nearZ = nearZ; dofParam_->farZ = farZ; }

private:
    /// <summary>
    /// 被写界深度パス用の定数バッファ構造体。  
    /// カメラの深度情報およびボケ強度を制御するパラメータを保持する。
    /// </summary>
    struct DoFPassParam
    {
        float nearZ = 0.1f;         ///< カメラの Near クリップ面。
        float farZ = 1000.0f;       ///< カメラの Far クリップ面。
        float focusDepth = 4.0f;    ///< フォーカスが合う深度距離。
        float blurScale = 1.0f;     ///< COC → ブラー半径変換係数（ブラー強度スケール）。
    };

    /// <summary>被写界深度用のパラメータ。</summary>
    std::shared_ptr<DoFPassParam> dofParam_;

    /// <summary>コピー処理を担当する CopyPass。</summary>
    CopyPass copyPass_;
};

