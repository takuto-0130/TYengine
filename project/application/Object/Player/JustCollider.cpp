#include "JustCollider.h"
#include "Player.h"
#include "../ColliderTypeID/ColliderTypeID.h"

void JustCollider::OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void JustCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	if(other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::E_BULLET))
	{
		player_->OnJust();
	}
}

void JustCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::E_BULLET))
	{
		player_->OffJust();
	}
}
