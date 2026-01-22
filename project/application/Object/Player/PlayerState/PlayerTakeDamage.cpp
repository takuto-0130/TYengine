#include "../Player.h"

void Player::InitTakeDamage()
{
	obj_->SetAddColor({ 1,1,1,1 });

	CameraShake::ShakeParams params;
	params.duration = 0.1f;
	params.amplitude = 0.1f;
	params.frequency = 20.0f;
	camera_->StartShake(params);
}

void Player::UpdateTakeDamage()
{
	if (GetStateElapsedTime() > 0.1f) ChangeState(PlayerState::ROOT);

	StartBarrelRoll();
}

void Player::ExitTakeDamage()
{
	obj_->SetAddColor({ 0,0,0,0 });
}