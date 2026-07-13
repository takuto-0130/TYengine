#include "../Player.h"
#include "../../../AppSystem/Audio/GameAudio.h"

using namespace TYEngine;
using namespace CameraSystem;

void PlayerStateTakeDamage::Init(Player& owner)
{
	GameAudio::GetInstance()->Play("damageP", false, SoundCategory::SE);
	// 被弾時に色を加算（白く光らせる）
	owner.obj_->SetAddColor(owner.takeDamage_.addColor);

	// カメラシェイクを開始
	CameraShake::ShakeParams params;
	params.duration = owner.takeDamage_.shakeDuration;
	params.amplitude = owner.takeDamage_.shakeAmplitude;
	params.frequency = owner.takeDamage_.shakeFrequency;
	owner.camera_->StartShake(params);
}

void PlayerStateTakeDamage::Update(Player& owner, float deltaTime)
{
	(void)deltaTime;
	// 一定時間経過後にROOTステートへ復帰
	if (owner.stateMachine_.GetStateElapsedTime() > owner.takeDamage_.recoveryTime) owner.stateMachine_.ChangeState(PlayerState::ROUTE);

	// 被弾中でもバレルロールは可能にする
	owner.StartBarrelRoll();
}

void PlayerStateTakeDamage::Exit(Player& owner)
{
	// 色加算をリセット
	owner.obj_->SetAddColor({ 0,0,0,0 });
}