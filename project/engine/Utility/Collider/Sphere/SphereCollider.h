#pragma once
#include "Collider.h"
#include "struct.h"

/// <summary>
/// 球状コライダークラス
/// </summary>
class SphereCollider :
    public Collider
{
public: // メンバ関数
    // コンストラクタ
    SphereCollider(uint32_t typeID, const Vector3& center, float radius)
        : Collider(typeID), sphere_(Sphere{ center,radius }) {}

    // getter
    ColliderShape GetShapeType() const override { return ColliderShape::SPHERE; }
    std::optional<Vector3> GetCenter() const override { return sphere_.center; }
    float GetRadius() const { return sphere_.radius; }
    Sphere GetSphere() const { return sphere_; }

    // 更新
    void Update(const Vector3& pos) override { sphere_.center = pos; }

private: // メンバ変数
    Sphere sphere_;
};

