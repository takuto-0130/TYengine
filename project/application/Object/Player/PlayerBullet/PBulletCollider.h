#pragma once
#include "Sphere/SphereCollider.h"
#include "../../BaseBullet/BaseBullet.h"
#include "../../ColliderTypeID/ColliderTypeID.h"

// プレイヤーの弾判定クラス
/// <summary>
/// プレイヤー弾用コライダー（球形状）。
/// 敵との衝突判定を行い、衝突時に弾側のコールバックを呼び出す。
/// </summary>
class PBulletCollider : 
    public TYEngine::Utility::SphereCollider
{
public:
    /// <summary>コンストラクタ。</summary>
    PBulletCollider(uint32_t typeID, const TYEngine::Utility::Vector3& center, float radius, BaseBullet* bullet)
        : SphereCollider(typeID, center, radius), bullet_(bullet) 
    {
    }

    /// <summary>衝突開始時の処理。</summary>
    void OnCollisionEnter([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override
    {
        // 敵に当たったら弾の衝突処理を実行
        if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::ENEMY))
        {
            bullet_->OnCollision();
        }
    }

    /// <summary>衝突中の処理。</summary>
    void OnCollisionStay([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override
    {
    }

    /// <summary>衝突終了時の処理。</summary>
    void OnCollisionExit([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override
    {
    }

private:
    BaseBullet* bullet_ = nullptr;
};
