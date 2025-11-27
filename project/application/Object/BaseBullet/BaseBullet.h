#pragma once
#include "BaseObject.h"
class BaseBullet :
    public BaseObject
{
public:
	// ワールド座標を取得
	virtual Vector3 GetWorldPosition()
	{
		Vector3 worldPos;
		worldPos.x = worldTransform_.matWorld_.m[3][0];
		worldPos.y = worldTransform_.matWorld_.m[3][1];
		worldPos.z = worldTransform_.matWorld_.m[3][2];
		return worldPos;
	}

	// ローカルスケールをセット
	virtual void SetScale(const Vector3& scale) { worldTransform_.colliderScale_ = scale; }

	// ローカル回転をセット
	virtual void SetRotate(const Vector3& rota) { worldTransform_.rotation_ = rota; }

	// ローカル座標をセット
	virtual void SetTranslation(const Vector3& pos) { worldTransform_.translation_ = pos; }

	// 飛んでいく向き
	virtual void SetShotDirection(const Vector3& direction) { direction_ = direction; }

	// 死んだかどうか
	virtual bool IsDead() const { return isDead_; }

	// 衝突時処理
	virtual void OnCollision() { isDead_ = true; }

protected:
	// 死亡判定フラグ
	bool isDead_ = false;

	// コライダー用スケール
	float colliderScale_ = 0.1f;

	float scale_ = 0.05f;

	// そのフレームの移動量
	Vector3 velocity_ = {};

	// 標準速度
	float defaultSpeed_ = 1.0f;

	// 移動方向
	Vector3 direction_ = { 0,0,1 };

	// 弾の寿命
	float lifeTime_ = 5.0f;
};

