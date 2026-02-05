#include "PlayerCollider.h"
#include "Player.h"
#include "../ColliderTypeID/ColliderTypeID.h"

using namespace TYEngine::Utility;

void PlayerCollider::OnCollisionEnter([[maybe_unused]]Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
	// 敵弾に当たった場合（バレルロール中は無敵）
	if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::E_BULLET) && player_->GetCurrentState() != PlayerState::BARREL_ROLL)
	{
		// プレイヤーを赤く点滅させ、ダメージ処理を呼ぶ
		player_->GetObj()->SetColor({ 1,0.1f,0.1f,1.0f });
		player_->TakeDamage();
	}
}

void PlayerCollider::OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}

void PlayerCollider::OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info)
{
}
