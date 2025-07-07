#pragma once
#include "../Collider.h"
class SphereCollider :
    public Collider
{
public:
    SphereCollider(uint32_t typeID, const Vector3& center, float radius)
        : Collider(typeID, center), radius_(radius) {
    }

    ColliderShape GetShapeType() const override { return ColliderShape::SPHERE; }

    Vector3 GetCenter() const override { return center_; }

    float GetRadius() const { return radius_; }

    void Update(const Vector3& pos) override { center_ = pos; }

private:
    float radius_;
};

