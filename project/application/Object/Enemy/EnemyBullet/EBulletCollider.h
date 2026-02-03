#pragma once
#include "Sphere/SphereCollider.h"
#include "../../BaseBullet/BaseBullet.h"
#include "../../ColliderTypeID/ColliderTypeID.h"

// 敵の弾判定クラス
/// <summary>
/// 敵弾用コライダー（球形状）。
/// プレイヤーとの衝突判定を行い、衝突時に弾側のコールバックを呼び出す。
/// </summary>
class EBulletCollider : public SphereCollider
{
public:
	/// <summary>コンストラクタ。</summary>
	EBulletCollider(uint32_t typeID, const Vector3& center, float radius, BaseBullet* bullet)
		: SphereCollider(typeID, center, radius), bullet_(bullet)
	{
	}

	/// <summary>衝突開始時の処理。</summary>
	void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override
	{
		// プレイヤーと衝突した場合、弾の衝突処理を実行
		if (other.GetTypeID() == static_cast<uint32_t>(ColliderTypeID::PLAYER))
		{
			bullet_->OnCollision();
		}
	}

	/// <summary>衝突中の処理。</summary>
	void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override
	{
	}

	/// <summary>衝突終了時の処理。</summary>
	void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) override
	{
	}

private:
	BaseBullet* bullet_ = nullptr;
};
