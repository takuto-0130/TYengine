#include "PlayerBulletNormal.h"

void PlayerBulletNormal::InitAfterCollision()
{
	// 着弾後は即座に消滅（エフェクトなどはOnCollisionで生成済み）
	isDead_ = true;
}

void PlayerBulletNormal::UpdateAfterCollision()
{
}

void PlayerBulletNormal::ExitAfterCollision()
{
}
