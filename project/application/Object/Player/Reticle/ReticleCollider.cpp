#include "ReticleCollider.h"
#include "Reticle.h"
#include "../../ColliderTypeID/ColliderTypeID.h"
#include "Sphere/SphereCollider.h"

void ReticleCollider::OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
    if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::ENEMY)) 
    {
        if (const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(&other))
        {
            float distance = Length(
                GetRay().origin - GetRaySphereIntersectionPoint(GetRay(), sphere->GetSphere())
            );
            reticle_->SetFrameDistance(distance);
        }
    }
}

void ReticleCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void ReticleCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

Vector3 ReticleCollider::GetRaySphereIntersectionPoint(const Ray& ray, const Sphere& sphere) const
{
    Vector3 m = ray.origin - sphere.center;
    float b = Dot(ray.diff, m);
    float c = Dot(m, m) - sphere.radius * sphere.radius;
    float discriminant = b * b - c;

    // 衝突している前提なので、discriminant > 0
    float t = -b - sqrtf(discriminant);

    return ray.origin + ray.diff * t;
}
