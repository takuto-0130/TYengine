#include "Linear.h"

void EnemyBullet::Linear::InitAfterCollision()
{
	// 着弾後は即座に死亡
	isDead_ = true;
}

void EnemyBullet::Linear::UpdateAfterCollision()
{
}

void EnemyBullet::Linear::ExitAfterCollision()
{
}