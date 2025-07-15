#include "PlayerBulletNormal.h"

void PlayerBulletNormal::InitLiner()
{
}

void PlayerBulletNormal::UpdateLiner()
{
	// 寿命を超えたら死亡フラグをtrueに
	if (GetStateElapsedTime() > lifeTime_) 
	{
		isDead_ = true;
	}

	Move();
	RotationDirection();

	worldTransform_.TransferMatrix();
}

void PlayerBulletNormal::ExitLiner()
{
}

void PlayerBulletNormal::Move()
{
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	worldTransform_.translation_ += velocity_;
}

void PlayerBulletNormal::RotationDirection()
{
	// Z軸向き（前方）からY軸回転（Yaw）を計算（XZ平面で）
	float yaw = std::atan2(direction_.x, direction_.z);

	// 上下回転（Pitch）も反映したいならY除いたベクトル長からPitchを算出
	float lenXZ = std::sqrt(direction_.x * direction_.x + direction_.z * direction_.z);
	float pitch = std::atan2(-direction_.y, lenXZ);

	// Roll（横傾き）は普通は 0 でOK（必要な場合だけ）
	float roll = 0.0f;

	// プレイヤーに回転を適用（Y軸回転のみ、またはX,Y）
	worldTransform_.rotation_ = { pitch, yaw, roll }; // ← 自由に調整可能
	worldTransform_.TransferMatrix();
}
