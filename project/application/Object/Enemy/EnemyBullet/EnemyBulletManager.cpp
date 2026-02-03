#include "EnemyBulletManager.h"


void EnemyBulletManager::Init()
{
	bullets_.reserve(256); // メモリ再確保があまり発生しないようある程度のサイズを確保
}

void EnemyBulletManager::Update()
{
	// 各弾の更新
    for (auto& b : bullets_) 
    {
        b->Update();
    }

    // 死亡した弾を削除・メモリ解放
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](auto& b) { return b->IsDead(); }),
        bullets_.end()
    );
}

void EnemyBulletManager::Draw()
{
    for (auto& b : bullets_)
    {
        b->Draw();
    }
}
