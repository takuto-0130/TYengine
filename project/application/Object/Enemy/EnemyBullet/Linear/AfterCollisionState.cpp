#include "Linear.h"

namespace EnemyBullet
{
	void LinearStateAfterCollision::Init(Linear& owner)
	{
		// 着弾後は即座に死亡
		owner.isDead_ = true;
	}

	void LinearStateAfterCollision::Update(Linear&, float) {}

	void LinearStateAfterCollision::Exit(Linear&) {}
}