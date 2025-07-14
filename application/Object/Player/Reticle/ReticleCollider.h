#pragma once
#include "Ray/RayCollider.h"

class ReticleCollider :
	public RayCollider
{
public:
	ReticleCollider(uint32_t typeID, const Vector3& origin, Vector3 dir)
		: RayCollider(typeID, origin, dir) {
	}

	void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
	void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
	void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override;
};

