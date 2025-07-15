#pragma once
#include "Collider.h"
#include "struct.h"
class RayCollider :
    public Collider
{
public:
    RayCollider(uint32_t typeID, const Vector3& origin, Vector3 dir)
        : Collider(typeID), ray_(Ray{ origin,dir })
    {
    }

    ColliderShape GetShapeType() const override { return ColliderShape::RAY; }

    std::optional<Vector3> GetCenter() const override { return ray_.origin; }

    Vector3 GetDiff() const { return ray_.diff; }

    Ray GetRay() const { return ray_; }

    void Update(const Vector3& pos) override { ray_.origin = pos; }

    void SetDirection(const Vector3& dir) { ray_.diff = dir; }

private:
    Ray ray_;
};

