#include "../Player.h"

void Player::InitRoute()
{

}

void Player::UpdateRoute()
{
	// 通常移動処理
	Move();

	// バレルロール遷移判定
	StartBarrelRoll();
}

void Player::ExitRoute()
{

}