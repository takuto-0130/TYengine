#pragma once
#include "IAttackStrategy.h"
#include "../EnemyBullet/Linear/Linear.h"
#include "../EnemyBullet/EnemyBulletManager.h"
#include <cmath>
#include "../Enemy.h"


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
			float offset = Enemy::jm_.Get<float>("enemy.attack.verticalOffset", 0.02f);
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + up * offset));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + up * -offset));
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
			float offsetInner = Enemy::jm_.Get<float>("enemy.attack.horizontalOffsetInner", 0.02f);
			float offsetOuter = Enemy::jm_.Get<float>("enemy.attack.horizontalOffsetOuter", 0.06f);
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * offsetOuter));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * offsetInner));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * -offsetInner));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * -offsetOuter));
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
			float offsetUp = Enemy::jm_.Get<float>("enemy.attack.triangleOffsetUp", 0.04f);
			float offsetRight = Enemy::jm_.Get<float>("enemy.attack.triangleOffsetRight", 0.04f);
			float offsetDown = Enemy::jm_.Get<float>("enemy.attack.triangleOffsetDown", -0.02f);
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + up * offsetUp));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * offsetRight + up * offsetDown));
			SpawnBullet(manager, origin, TYEngine::Utility::Normalize(forward + right * -offsetRight + up * offsetDown));
		}
	};
}