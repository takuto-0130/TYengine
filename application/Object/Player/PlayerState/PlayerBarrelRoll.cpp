#include "../Player.h"
#include "input.h"
#include "Ease.h"
#include "BulletTimeController.h"

void Player::InitBarrelRoll()
{
	startRollPos_ = screenOffset_;
}

void Player::UpdateBarrelRoll()
{
	BarrelRoll();
}

void Player::ExitBarrelRoll()
{

}



void Player::StartBarrelRoll()
{
	if(input_->TriggerKey(DIK_LSHIFT))
	{
		ChangeState(PlayerState::BARREL_ROLL);
		BulletTimeController::GetInstance()->Trigger(
			0.05f, 0.0f, 1.0f, 0.8f,
			EaseFixed::InQuart, EaseFixed::OutQuart);
	}
}

void Player::BarrelRoll()
{
	if (inputDir_.x != 0)
	{
		if (inputDir_.x < 0)
		{
			LeftRoll();
		}
		if (inputDir_.x > 0)
		{
			RightRoll();
		}
	}
	else if (inputDir_.y != 0)
	{
		if (inputDir_.y < 0)
		{
			LeftRoll();
		}
		if (inputDir_.y > 0)
		{
			RightRoll();
		}
	}

	ClampOffset();

	Vector3 worldPos = ConvertScreenOffsetToWorld(screenOffset_);
	worldTransform_.translation_ = worldPos;

	if (GetStateElapsedTime() >= rollTime_ || Length(inputDir_) == 0.0f)
	{
		ChangeState(PlayerState::ROOT);
	}
}



void Player::LeftRoll()
{
	goalRollPos_ = startRollPos_ + inputDir_ * rollRange_;
	float t = GetStateElapsedTime() / rollTime_;
	t = std::clamp(t, 0.0f, 1.0f);
	screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
	roll = Lerp(0.0f, leftRoll_, EaseFixed::OutBack(t));
}

void Player::RightRoll()
{
	goalRollPos_ = startRollPos_ + inputDir_ * rollRange_;
	float t = GetStateElapsedTime() / rollTime_;
	t = std::clamp(t, 0.0f, 1.0f);
	screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
	roll = Lerp(0.0f, rightRoll_, EaseFixed::OutBack(t));
}