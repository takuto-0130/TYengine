#include "Liner.h"

void EnemyBullet::Liner::InitShot()
{
}

void EnemyBullet::Liner::UpdateShot()
{
#ifdef _DEBUG
	ImGui::Begin("Bullet");
	ImGui::Text("%.2f,%.2f,%.2f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
	ImGui::Text("%.5f", deltaTime_);
	ImGui::End();
#endif // _DEBUG
	// 寿命を超えたら死亡フラグをtrueに
	if (GetStateElapsedTime() > lifeTime_)
	{
		isDead_ = true;
	}

	Move();
	RotationDirection();

	worldTransform_.TransferMatrix();
}

void EnemyBullet::Liner::ExitShot()
{
}

void EnemyBullet::Liner::Move()
{
	velocity_ = direction_ * defaultSpeed_ * deltaTime_;

	worldTransform_.translation_ += velocity_;
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
	worldTransform_.rotation_ = { pitch, yaw, roll };
	worldTransform_.TransferMatrix();
}