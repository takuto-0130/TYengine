#pragma once
#include "Sphere/SphereCollider.h"

class Enemy;
class EnemyCollider :
    public SphereCollider
{
public:
    EnemyCollider(uint32_t typeID, const Vector3& center, float radius, Enemy* enemy)
        : SphereCollider(typeID, center, radius), enemy_(enemy)
    {
    }

    // 衝突イベントコールバック（必要に応じて 'bullet_' に処理を委譲）
    void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;

    void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;

    void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;

private:
    Enemy* enemy_ = nullptr;
};

