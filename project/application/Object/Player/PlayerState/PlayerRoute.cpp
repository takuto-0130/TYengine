#include "../Player.h"

void PlayerStateRoute::Init(Player& owner)
{
	(void)owner;
}

void PlayerStateRoute::Update(Player& owner, float deltaTime)
{
	(void)deltaTime;
	// 通常移動処理
	owner.Move();

	// バレルロール遷移判定
	owner.StartBarrelRoll();
}

void PlayerStateRoute::Exit(Player& owner)
{
	(void)owner;
}