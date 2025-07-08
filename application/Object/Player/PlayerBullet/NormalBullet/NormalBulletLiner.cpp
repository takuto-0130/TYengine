#include "PlayerBulletNormal.h"

void PlayerBulletNormal::InitLiner()
{
}

void PlayerBulletNormal::UpdateLiner()
{
	if (GetStateElapsedTime() < lifeTime_) isDead_ = true;

	Move();

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