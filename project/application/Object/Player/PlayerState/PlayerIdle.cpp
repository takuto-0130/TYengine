#include "../Player.h"

void PlayerStateIdle::Init(Player& owner)
{
	(void)owner;
}

void PlayerStateIdle::Update(Player& owner, float deltaTime)
{
	(void)owner;
	(void)deltaTime;
}

void PlayerStateIdle::Exit(Player& owner)
{
	(void)owner;
}