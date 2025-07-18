#include "../Player.h"
#include "input.h"

void Player::InitBarrelRoll()
{

}

void Player::UpdateBarrelRoll()
{

}

void Player::ExitBarrelRoll()
{

}



void Player::StartBarrelRoll()
{
	if(input_->TriggerKey(DIK_LSHIFT))
	{
		ChangeState(PlayerState::BARREL_ROLL);
	}
}

void Player::BarrelRoll()
{
	
}