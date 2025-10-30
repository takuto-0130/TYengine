#include "PlayerCollider.h"
#include "Player.h"
#include "../ColliderTypeID/ColliderTypeID.h"

void PlayerCollider::OnCollisionEnter([[maybe_unused]]Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::E_BULLET))
	{
		player_->GetObj()->SetColor({ 1,0.1f,0.1f,1.0f });
		player_->OnCollision();
	}
}

void PlayerCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void PlayerCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}
