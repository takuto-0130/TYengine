#include "Linear.h"

namespace EnemyBullet
{
	void Linear::InitAfterCollision()
	{
		// 着弾後は即座に死亡
		isDead_ = true;
	}

	void Linear::UpdateAfterCollision(){}

	void Linear::ExitAfterCollision(){}
}