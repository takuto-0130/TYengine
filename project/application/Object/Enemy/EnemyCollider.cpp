#include "EnemyCollider.h"
#include "Enemy.h"
#include "../ColliderTypeID/ColliderTypeID.h"

void EnemyCollider::OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::P_BULLET))
	{
		enemy_->OnCollision();
	}
}

void EnemyCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void EnemyCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}
