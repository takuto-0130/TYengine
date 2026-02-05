#pragma once
#include "Sphere/SphereCollider.h"

class Enemy;
/// <summary>
/// 敵キャラクター用のコライダー（球形状）。
/// SphereCollider を継承し、衝突時に敵本体（Enemyクラス）への通知を行う機能を持つ。
/// </summary>
class EnemyCollider :
    public TYEngine::Utility::SphereCollider
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="typeID">コライダーのタイプID。</param>
    /// <param name="center">中心座標。</param>
    /// <param name="radius">半径。</param>
    /// <param name="enemy">所有者となるEnemyインスタンス。</param>
    EnemyCollider(uint32_t typeID, const TYEngine::Utility::Vector3& center, float radius, Enemy* enemy)
        : SphereCollider(typeID, center, radius), enemy_(enemy)
    {
    }

    /// <summary>衝突開始時の処理。</summary>
    void OnCollisionEnter([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;

    /// <summary>衝突中の処理。</summary>
    void OnCollisionStay([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;

    /// <summary>衝突終了時の処理。</summary>
    void OnCollisionExit([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;

private:
    /// <summary>このコライダーを所有する敵オブジェクト。</summary>
    Enemy* enemy_ = nullptr;
};

