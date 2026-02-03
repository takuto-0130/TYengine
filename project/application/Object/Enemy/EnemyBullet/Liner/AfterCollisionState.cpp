#include "Liner.h"

void EnemyBullet::Liner::InitAfterCollision()
{
	// 着弾後は即座に死亡
	isDead_ = true;
}

void EnemyBullet::Liner::UpdateAfterCollision()
{
}

void EnemyBullet::Liner::ExitAfterCollision()
{
}