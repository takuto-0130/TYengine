#include "../Player.h"

using namespace TYEngine::CameraSystem;

void Player::InitTakeDamage()
{
	// 被弾時に色を加算（白く光らせる）
	obj_->SetAddColor({ 1,1,1,1 });

	// カメラシェイクを開始
	CameraShake::ShakeParams params;
	params.duration = 0.1f;
	params.amplitude = 0.1f;
	params.frequency = 20.0f;
	camera_->StartShake(params);
}

void Player::UpdateTakeDamage()
{
	// 一定時間経過後にROOTステートへ復帰
	if (stateMachine_.GetStateElapsedTime() > 0.1f) stateMachine_.ChangeState(PlayerState::ROUTE);

	// 被弾中でもバレルロールは可能にする
	StartBarrelRoll();
}

void Player::ExitTakeDamage()
{
	// 色加算をリセット
	obj_->SetAddColor({ 0,0,0,0 });
}