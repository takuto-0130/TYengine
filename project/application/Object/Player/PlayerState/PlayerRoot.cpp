#include "../Player.h"

void Player::InitRoot()
{

}

void Player::UpdateRoot()
{
	// 通常移動処理
	Move();

	// バレルロール遷移判定
	StartBarrelRoll();
}

void Player::ExitRoot()
{

}