#pragma once
#include "../Collider.h"
class SphereCollider :
    public Collider
{
    SphereCollider(const Vector3& center, float radius, uint32_t typeID)
        : Collider(typeID), center_(center), radius_(radius) {
    }

    Vector3 GetCenter() const override { return center_; }

    void Update(const Vector3& pos) override { center_ = pos; }

    bool CheckCollision(const Collider& other) const override {
        return other.CheckCollisionWith(*this);
    }

    bool CheckCollisionWith(const SphereCollider& other) const override {
        float distSq = Length(center_ - other.center_);
        return distSq <= (radius_ + other.radius_);
    }

private:
    Vector3 center_;
    float radius_;
};

