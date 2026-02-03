#include "PlayerBulletNormal.h"

void PlayerBulletNormal::InitLiner()
{
}

void PlayerBulletNormal::UpdateLiner()
{
	// 寿命を超えたら死亡フラグをtrueにし、削除対象とする
	if (GetStateElapsedTime() > lifeTime_) 
	{
		isDead_ = true;
	}

	// 移動処理
	Move();
	// 進行方向に合わせて回転
	RotationDirection();

	worldTransform_.Update();
}

void PlayerBulletNormal::ExitLiner()
{
}

void PlayerBulletNormal::Move()
{
	// 速度ベクトルの計算
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	// 座標更新
	worldTransform_.SetTranslation(worldTransform_.GetTranslation() + velocity_);
}

void PlayerBulletNormal::RotationDirection()
{
	// 進行方向（Y軸回転 Yaw）を計算
	float yaw = std::atan2(direction_.x, direction_.z);

	// 進行方向（X軸回転 Pitch）を計算
	float lenXZ = std::sqrt(direction_.x * direction_.x + direction_.z * direction_.z);
	float pitch = std::atan2(-direction_.y, lenXZ);

	// Roll（横傾き）は固定
	float roll = 0.0f;

	// 回転を適用
	worldTransform_.SetRotate({ pitch, yaw, roll });
	worldTransform_.Update();
}
