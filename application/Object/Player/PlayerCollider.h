#pragma once
#include "Sphere/SphereCollider.h"

class Player;
class PlayerCollider :
    public SphereCollider
{
public:
    PlayerCollider(uint32_t typeID, const Vector3& center, float radius, Player* player)
        : SphereCollider(typeID, center, radius), player_(player) {
    }

    void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
    void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
    void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;

private:
    Player* player_ = nullptr;
};

