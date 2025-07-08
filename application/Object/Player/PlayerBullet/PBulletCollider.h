#pragma once
#include "Sphere/SphereCollider.h"

class PlayerBullet;
class PBulletCollider :
    public SphereCollider
{
public:
    PBulletCollider(uint32_t typeID, const Vector3& center, float radius, PlayerBullet* bullet)
        : SphereCollider(typeID, center, radius), bullet_(bullet) {
    }

    void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
    void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
    void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;

private:
    PlayerBullet* bullet_ = nullptr;
};

