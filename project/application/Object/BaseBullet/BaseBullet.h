#pragma once
#include "BaseObject.h"
class BaseBullet :
    public BaseObject
{
public:
	// 飛んでいく向き
	virtual void SetShotDirection(const Vector3& direction) { direction_ = direction; }

	// 死んだかどうか
	virtual bool IsDead() const { return isDead_; }

	// 衝突時処理
	virtual void OnCollision() { isDead_ = true; }

	void SetCamera(Camera* camera) { camera_ = camera; }

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

	// カメラのポインタ
	Camera* camera_ = nullptr;
};

