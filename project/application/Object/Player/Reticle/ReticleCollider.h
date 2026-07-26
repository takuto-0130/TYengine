#pragma once
#include "Ray/RayCollider.h"

class Reticle;
/// <summary>
/// レティクル（照準）用のコライダー（レイ形状）。
/// 敵などとの衝突判定に使用される。
/// </summary>
class ReticleCollider :
	public TYEngine::Utility::RayCollider
{
public:
	/// <summary>コンストラクタ。</summary>
	ReticleCollider(uint32_t typeID, const TYEngine::Utility::Vector3& origin, const TYEngine::Utility::Vector3& dir, Reticle* reticle)
		: RayCollider(typeID, origin, dir), reticle_(reticle) {
	}

	/// <summary>衝突開始時の処理。</summary>
	void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;
	/// <summary>衝突中の処理。</summary>
	void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;
	/// <summary>衝突終了時の処理。</summary>
	void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;

private:
	/// <summary>レイと球の交点を計算するヘルパー関数。</summary>
	TYEngine::Utility::Vector3 GetRaySphereIntersectionPoint(const TYEngine::Utility::Ray& ray, const TYEngine::Utility::Sphere& sphere) const;
	
private:
	/// <summary>所有者（レティクル）。</summary>
	Reticle* reticle_ = nullptr;
};

