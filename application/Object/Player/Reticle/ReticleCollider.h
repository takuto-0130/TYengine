#pragma once
#include "Ray/RayCollider.h"

class Reticle;
class ReticleCollider :
	public RayCollider
{
public:
	ReticleCollider(uint32_t typeID, const Vector3& origin, Vector3 dir, Reticle* reticle)
		: RayCollider(typeID, origin, dir), reticle_(reticle) {
	}

	void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
	void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
	void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;

private:
	Vector3 GetRaySphereIntersectionPoint(const Ray& ray, const Sphere& sphere) const;
	
private:
	Reticle* reticle_ = nullptr;
};

