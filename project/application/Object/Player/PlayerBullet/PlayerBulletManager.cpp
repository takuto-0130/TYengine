#include "PlayerBulletManager.h"
#include "NormalBullet/PlayerBulletNormal.h"
#include "HomingBullet/PlayerBulletHoming.h"
#include "../../Enemy/EnemyManager/EnemyManager.h"

using namespace TYEngine::Utility;

PlayerBulletManager::PlayerBulletManager(Player* player)
{
	player_ = player;
}

void PlayerBulletManager::Init()
{
}

void PlayerBulletManager::Update()
{
	// 全弾の更新処理を実行
	for (auto&& bullet : bullets_)
	{
		bullet->Update();
	}
}

void PlayerBulletManager::Draw()
{
	// 死亡判定が出た弾をリストから削除・メモリ解放
	bullets_.remove_if([](const std::unique_ptr<BaseBullet>& b) { return b->IsDead(); });

	// 残存している弾の描画
	for (auto&& bullet : bullets_)
	{
		bullet->Draw();
	}
}

void PlayerBulletManager::Fire(PlayerBulletType type, const Vector3& pos, const Vector3& direction, Enemy* target, EnemyManager* mgr)
{
	using enum PlayerBulletType;
	std::unique_ptr<BaseBullet> bullet;
	switch (type)
	{
	case NORMAL:
		// 通常弾の生成と初期設定
		bullet = std::make_unique<PlayerBulletNormal>();
		bullet->SetCamera(camera_);
		dynamic_cast<PlayerBulletNormal*>(bullet.get())->SetJM(jm_);
		bullet->Init();
		bullet->SetTranslation(pos);
		bullet->SetShotDirection(direction);

		bullets_.push_back(std::move(bullet));
		break;
	case HOMING:
		// ホーミング弾の生成と初期設定
		bullet = std::make_unique<PlayerBulletHoming>();
		bullet->SetCamera(camera_);
		dynamic_cast<PlayerBulletHoming*>(bullet.get())->SetJM(jm_);
		bullet->Init();
		bullet->SetTranslation(pos);
		bullet->SetShotDirection(direction);
		if(mgr)
		{
			if (mgr->IsValidEnemy(target)) bullet->SetTargetObject(target);
			dynamic_cast<PlayerBulletHoming*>(bullet.get())->SetEnemyManager(mgr);
		}

		bullets_.push_back(std::move(bullet));
		break;
	case CHARGE:
		// チャージ弾（未実装）
		break;
	}
}
