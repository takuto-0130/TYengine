#pragma once
#include "IParticleRenderer.h"
#include "SingletonObject.h"
#include <vector>
#include <memory>



/// <summary>
/// パーティクルシステムの管理クラス（シングルトン）。
/// 複数のパーティクルレンダラー（IParticleRenderer）を保持・更新・描画する。
/// </summary>
class ParticleManager :
    public SingletonObject<ParticleManager>
{
    friend class SingletonObject<ParticleManager>;
    friend struct std::default_delete<ParticleManager>;

private:
    // 外部からの new/delete を禁止
    ParticleManager() = default;
    ~ParticleManager() = default;

public:
    /// <summary>
    /// 新しいパーティクルレンダラーを追加・登録する。
    /// </summary>
    /// <param name="particle">追加するパーティクルレンダラー（所有権を移動）。</param>
    /// <returns>登録されたインデックス。</returns>
    int Add(std::unique_ptr<IParticleRenderer> particle);

    /// <summary>
    /// 登録されている全てのパーティクルレンダラーを初期化する。
    /// </summary>
    /// <param name="dx">DirectX 基盤。</param>
    /// <param name="srv">SRV 管理クラス。</param>
    /// <param name="cam">カメラ。</param>
    void InitializeAll(DirectXBasis* dx, SrvManager* srv, Camera* cam);

    /// <summary>
    /// 全てのパーティクルレンダラーを更新する。
    /// </summary>
    void UpdateAll();

    /// <summary>
    /// 全てのパーティクルを描画する。
    /// </summary>
    void DrawAll();

public:
    /// <summary>
    /// 指定インデックスのパーティクルエミッター設定を更新する。
    /// </summary>
    /// <param name="index">パーティクルインデックス。</param>
    /// <param name="emitter">新しいエミッター設定。</param>
    void SetEmitter(int index, IParticleRenderer::Emitter& emitter);

    /// <summary>
    /// 指定インデックスのパーティクル放出（Emit）をトリガーする。
    /// </summary>
    /// <param name="index">パーティクルインデックス。</param>
    /// <param name="flag">true なら放出処理を実行。</param>
    void TriggerEmit(int index, bool flag);

private:
    /// <summary>管理下のパーティクルレンダラー配列。</summary>
    std::vector<std::unique_ptr<IParticleRenderer>> particles_;
};
