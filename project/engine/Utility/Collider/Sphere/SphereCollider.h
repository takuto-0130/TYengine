#pragma once
#include "Collider.h"
#include "struct.h"
class SphereCollider :
    public Collider
{
public:
    SphereCollider(uint32_t typeID, const Vector3& center, float radius)
        : Collider(typeID), sphere_(Sphere{ center,radius })
    {
    }

    ColliderShape GetShapeType() const override { return ColliderShape::SPHERE; }

    std::optional<Vector3> GetCenter() const override { return sphere_.center; }

    float GetRadius() const { return sphere_.radius; }

    Sphere GetSphere() const { return sphere_; }

    void Update(const Vector3& pos) override { sphere_.center = pos; }

private:
    Sphere sphere_;
};

