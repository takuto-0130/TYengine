#include "Linear.h"

void EnemyBullet::Linear::InitShot()
{
}

void EnemyBullet::Linear::UpdateShot()
{
	// 寿命を超えたら死亡フラグを true に
	if (GetStateElapsedTime() > lifeTime_)
	{
		isDead_ = true;
	}

	// 移動処理
	Move();
	// 回転処理
	RotationDirection();

	worldTransform_.Update();
}

void EnemyBullet::Linear::ExitShot()
{
}

void EnemyBullet::Linear::Move()
{
	// 速度ベクトル算出
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	// 座標更新
	worldTransform_.SetTranslation(worldTransform_.GetTranslation() + velocity_);
}

void EnemyBullet::Linear::RotationDirection()
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