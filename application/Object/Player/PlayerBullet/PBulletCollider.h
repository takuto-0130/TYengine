#pragma once
#include "Sphere/SphereCollider.h"
#include "../../BaseBullet/BaseBullet.h"
#include "../../ColliderTypeID/ColliderTypeID.h"

// プレイヤーの弾判定クラス
class PBulletCollider : public SphereCollider
{
public:
    PBulletCollider(uint32_t typeID, const Vector3& center, float radius, BaseBullet* bullet)
        : SphereCollider(typeID, center, radius), bullet_(bullet) 
    {
    }

    // 衝突イベントコールバック（必要に応じて 'bullet_' に処理を委譲）
    void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override
    {
        if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::ENEMY))
        {
            bullet_->OnCollision();
        }
    }

    void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override
    {
    }

    void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override
    {
    }

private:
    BaseBullet* bullet_ = nullptr;
};
