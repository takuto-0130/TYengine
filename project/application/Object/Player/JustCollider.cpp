#include "JustCollider.h"
#include "Player.h"
#include "../ColliderTypeID/ColliderTypeID.h"

void JustCollider::OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void JustCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	// 敵弾がジャスト回避エリア内にある場合
	if(other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::E_BULLET))
	{
		player_->OnJust();
	}
}

void JustCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	// 弾がエリアを出たらジャスト回避状態を解除
	if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::E_BULLET))
	{
		player_->OffJust();
	}
}
