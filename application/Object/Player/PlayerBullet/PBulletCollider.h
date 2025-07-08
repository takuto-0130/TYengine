#pragma once
#include "Sphere/SphereCollider.h"

// プレイヤーの弾判定クラス（テンプレート）
// 'BulletClass' = 弾の継承クラス
template<typename BulletClass>
class PBulletCollider : public SphereCollider
{
public:
    PBulletCollider(uint32_t typeID, const Vector3& center, float radius, BulletClass* bullet)
        : SphereCollider(typeID, center, radius), bullet_(bullet) 
    {
    }

    // 衝突イベントコールバック（必要に応じて 'bullet_' に処理を委譲）
    void OnCollisionEnter(Collider& other, const CollisionInfo& info) override 
    {
    }

    void OnCollisionStay(Collider& other, const CollisionInfo& info) override 
    {
    }

    void OnCollisionExit(Collider& other, const CollisionInfo& info) override 
    {
    }

private:
    BulletClass* bullet_ = nullptr;
};
