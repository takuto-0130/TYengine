#pragma once
#include "PostEffectBase.h"

/// <summary>
/// 入力テクスチャをそのまま（または最小限の処理で）出力へコピーするポストエフェクト。  
/// デバッグ用途やポストエフェクトチェーンの基底パスとして使用する。
/// </summary>
namespace TYEngine {
namespace OffScreen {

using namespace Core; // For DirectXBasis

/// <summary>
/// 入力テクスチャをそのまま（または最小限の処理で）出力へコピーするポストエフェクト。  
/// デバッグ用途やポストエフェクトチェーンの基底パスとして使用する。
/// </summary>
class CopyImageEffect :
    public PostEffectBase
{
public:
    /// <summary>
    /// 初期化処理。  
    /// コピー用のシェーダ／定数バッファなどをセットアップする。
    /// </summary>
    /// <param name="dx">DirectX 基盤。</param>
    /// <param name="srv">SRV 管理クラス。</param>
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    /// <summary>
    /// 毎フレームの更新処理。  
    /// 必要に応じて内部状態を更新する（通常は処理なし）。
    /// </summary>
    void Update() override;

    /// <summary>
    /// ImGui によるデバッグ UI 表示。  
    /// コピー動作の確認や将来的な拡張用に使用する。
    /// </summary>
    void ImGuiUpdate() override;

    /// <summary>
    /// 入力レンダーターゲットを出力へコピーする。
    /// </summary>
    /// <param name="input">コピー元のレンダーターゲット。</param>
    void Apply(RenderTexture* input) override;
};

} // namespace OffScreen
} // namespace TYEngine
