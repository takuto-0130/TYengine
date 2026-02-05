#pragma once
#include "IAttackStrategy.h"
#include "../EnemyBullet/Linear/Linear.h"
#include "../EnemyBullet/EnemyBulletManager.h"
#include <cmath>


// 敵の攻撃
namespace EnemyAttack
{
	// 共通ヘルパー: 弾を1発生成してマネージャに追加
	inline void SpawnBullet(EnemyBulletManager* manager, const TYEngine::Utility::Vector3& pos, const TYEngine::Utility::Vector3& dir)
	{
		auto bullet = std::make_unique<EnemyBullet::Linear>();
		bullet->Init();
		bullet->SetTranslation(pos);
		bullet->SetShotDirection(dir);
		manager->AddBullet(std::move(bullet));
	}
	// -------------------------------------------------------------
	// Type 0: 通常攻撃（プレイヤーに向かって1発）
	// -------------------------------------------------------------
	class NormalAttackStrategy : public IAttackStrategy
	{
	public:
		void Attack(const TYEngine::Utility::Vector3& origin, const TYEngine::Utility::Vector3& target, EnemyBulletManager* manager) override
		{
			TYEngine::Utility::Vector3 forward = TYEngine::Utility::Normalize(target - origin);
			SpawnBullet(manager, origin, forward);
		}
	};
	// -------------------------------------------------------------
	// Type 1: 垂直2点攻撃
	// -------------------------------------------------------------
	class VerticalSplitAttackStrategy : public IAttackStrategy
	{
	public:
		void Attack(const TYEngine::Utility::Vector3& origin, const TYEngine::Utility::Vector3& target, EnemyBulletManager* manager) override
		{
			TYEngine::Utility::Vector3 forward = TYEngine::Utility::Normalize(target - origin);

			// ローカル上方向を計算
			TYEngine::Utility::Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
			if (std::abs(forward.y) > 0.999f) worldUp = { 0.0f, 0.0f, 1.0f };
			TYEngine::Utility::Vector3 right = TYEngine::Utility::Normalize(Cross(worldUp, forward));
			TYEngine::Utility::Vector3 up = TYEngine::Utility::Normalize(Cross(forward, right));
			// 上下に少しずらして発射
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + up * 0.02f));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + up * -0.02f));
		}
	};
	// -------------------------------------------------------------
	// Type 2: 水平4点攻撃
	// -------------------------------------------------------------
	class HorizontalSplitAttackStrategy : public IAttackStrategy
	{
	public:
		void Attack(const TYEngine::Utility::Vector3& origin, const TYEngine::Utility::Vector3& target, EnemyBulletManager* manager) override
		{
			TYEngine::Utility::Vector3 forward = TYEngine::Utility::Normalize(target - origin);

			TYEngine::Utility::Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
			if (std::abs(forward.y) > 0.999f) worldUp = { 0.0f, 0.0f, 1.0f };
			TYEngine::Utility::Vector3 right = TYEngine::Utility::Normalize(Cross(worldUp, forward));
			// 左右にずらして4発発射
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * 0.06f));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * 0.02f));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * -0.02f));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * -0.06f));
		}
	};
	// -------------------------------------------------------------
	// Type 3: 三角3点攻撃
	// -------------------------------------------------------------
	class TriangleAttackStrategy : public IAttackStrategy
	{
	public:
		void Attack(const TYEngine::Utility::Vector3& origin, const TYEngine::Utility::Vector3& target, EnemyBulletManager* manager) override
		{
			TYEngine::Utility::Vector3 forward = Normalize(target - origin);

			TYEngine::Utility::Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
			if (std::abs(forward.y) > 0.999f) worldUp = { 0.0f, 0.0f, 1.0f };
			TYEngine::Utility::Vector3 right = TYEngine::Utility::Normalize(Cross(worldUp, forward));
			TYEngine::Utility::Vector3 up = TYEngine::Utility::Normalize(Cross(forward, right));
			// 左右にずらして4発発射
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + up * 0.04f));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * 0.04f + up * -0.02f));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * -0.04f + up * -0.02f));
		}
	};
}