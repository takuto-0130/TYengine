#include "Liner.h"

void EnemyBullet::Liner::InitShot()
{
}

void EnemyBullet::Liner::UpdateShot()
{
	// 寿命を超えたら死亡フラグを true に
	if (GetStateElapsedTime() > lifeTime_)
	{
		isDead_ = true;
	}

	Move();
	RotationDirection();

	worldTransform_.Update();
}

void EnemyBullet::Liner::ExitShot()
{
}

void EnemyBullet::Liner::Move()
{
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	worldTransform_.SetTranslation(worldTransform_.GetTranslation() + velocity_);
}

void EnemyBullet::Liner::RotationDirection()
{
	// Z軸向き（前方）からY軸回転（Yaw）を計算（XZ平面で）
	float yaw = std::atan2(direction_.x, direction_.z);

	// 上下回転（Pitch）も反映したいならY除いたベクトル長からPitchを算出
	float lenXZ = std::sqrt(direction_.x * direction_.x + direction_.z * direction_.z);
	float pitch = std::atan2(-direction_.y, lenXZ);

	// Roll（横傾き）
	float roll = 0.0f;

	// 回転を適用
	worldTransform_.SetRotate({ pitch, yaw, roll });
	worldTransform_.Update();
}