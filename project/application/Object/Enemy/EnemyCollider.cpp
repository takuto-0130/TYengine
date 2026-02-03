#include "EnemyCollider.h"
#include "Enemy.h"
#include "../ColliderTypeID/ColliderTypeID.h"

void EnemyCollider::OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	// プレイヤ―弾と接触し、かつ敵が生存・活動中の場合
	if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::P_BULLET) && !enemy_->IsDead() && enemy_->GetCurrentState() == EnemyState::ACTIVE)
	{
		// 敵側の衝突処理（被弾）を実行
		enemy_->OnCollision();
	}
}

void EnemyCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void EnemyCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}
