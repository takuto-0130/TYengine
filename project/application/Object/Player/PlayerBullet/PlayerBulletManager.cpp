#include "PlayerBulletManager.h"
#include "NormalBullet/PlayerBulletNormal.h"


PlayerBulletManager::PlayerBulletManager(Player* player)
{
	player_ = player;
}

void PlayerBulletManager::Init()
{
}

void PlayerBulletManager::Update()
{
	for (auto&& bullet : bullets_)
	{
		bullet->Update();
	}
}

void PlayerBulletManager::Draw()
{
	// 死亡判定が出た弾を削除
	bullets_.remove_if([](const std::unique_ptr<BaseBullet>& b) { return b->IsDead(); });

	for (auto&& bullet : bullets_)
	{
		bullet->Draw();
	}
}

void PlayerBulletManager::Fire(PlayerBulletType type, const Vector3& pos, const Vector3& direction)
{
	using enum PlayerBulletType;
	std::unique_ptr<BaseBullet> bullet;
	switch (type)
	{
	case NORMAL:
		bullet = std::make_unique<PlayerBulletNormal>();
		bullet->Init();
		bullet->SetTranslation(pos);
		bullet->SetShotDirection(direction);

		bullets_.push_back(std::move(bullet));
		break;
	case HOMING:


		break;
	case CHARGE:


		break;
	}
}
