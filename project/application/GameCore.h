#pragma once
#include "TYFrameWork.h"
#include "IScene.h"
#include "ParticleManager.h"

/// <summary>
/// ゲーム全体のメインエントリーポイント。  
/// TYFrameWork を継承し、ゲーム特有の初期化・更新・描画ロジックを実装する。
/// </summary>
class GameCore : public TYEngine::Framework::TYFramework
{
public:
    /// <summary>
    /// 初期化処理。  
    /// ゲームで使用する各種マネージャ（入力・パーティクル・シーンなど）を初期化する。
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了処理。  
    /// 使用している全リソースの解放やマネージャの終了処理を行う。
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 毎フレーム更新処理。  
    /// 入力状態やシーンの更新、パーティクルなどの動作を制御する。
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画処理。  
    /// フレーム全体のレンダリングを行う。
    /// </summary>
    void Draw() override;

private:
    /// <summary>入力管理クラス（キーボード、マウス、パッドなど）。</summary>
    TYEngine::Framework::Input* input_ = nullptr;

    /// <summary>パーティクル描画および管理クラス。</summary>
    TYEngine::Effect::ParticleManager* particleManager_ = nullptr;
};
