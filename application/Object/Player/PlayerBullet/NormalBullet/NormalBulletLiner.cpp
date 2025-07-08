#include "PlayerBulletNormal.h"

void PlayerBulletNormal::InitLiner()
{
}

void PlayerBulletNormal::UpdateLiner()
{
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