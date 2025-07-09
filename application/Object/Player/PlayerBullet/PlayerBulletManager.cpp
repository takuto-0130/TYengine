#include "PlayerBulletManager.h"
#include "../../BaseBullet/BaseBullet.h"


void PlayerBulletManager::Init(Player* player)
{
	player_ = player;
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
	for (auto&& bullet : bullets_)
	{
		bullet->Draw();
	}
}
