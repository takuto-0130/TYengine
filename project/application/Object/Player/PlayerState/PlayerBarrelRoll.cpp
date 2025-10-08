#include "../Player.h"
#include "input.h"
#include "Timer.h"
#include "Ease.h"
#include "BulletTimeController.h"
#include "PostEffectManager.h"
#include "VignetteEffect.h"
#include "RadialBlurEffect.h"

void Player::InitBarrelRoll()
{
	startRollPos_ = screenOffset_;
	rollEfectTimer_ = 0.0f;
	if(justRoll_)
	{
		PostEffectManager::GetInstance()->SetEffectEnabled("Vignette", true);
		PostEffectManager::GetInstance()->GetEffect<VignetteEffect>("Vignette")->SetPower(0.0f);
		PostEffectManager::GetInstance()->SetEffectEnabled("RadialBlur", true);
		PostEffectManager::GetInstance()->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.0f);
		PostEffectManager::GetInstance()->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter(screenOffset_);
	}
}

void Player::UpdateBarrelRoll()
{
	BarrelRoll();
}

void Player::ExitBarrelRoll()
{
	if (justRoll_)
	{
		PostEffectManager::GetInstance()->SetEffectEnabled("Vignette", false);
		PostEffectManager::GetInstance()->SetEffectEnabled("RadialBlur", false);
	}
}



void Player::StartBarrelRoll()
{
	if(input_->TriggerKey(DIK_LSHIFT))
	{
		ChangeState(PlayerState::BARREL_ROLL);
		justRoll_ = false;
		if(isJust_)
		{
			justRoll_ = true;
			BulletTimeController::GetInstance()->Trigger(
				0.05f, 0.0f, 1.0f, 0.8f,
				EaseFixed::InQuart, EaseFixed::OutQuart);
		}
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
	if (rollEfectTimer_ <= 2.1f)
	{
		rollEfectTimer_ += Timer::GetInstance()->GetRawDeltaTime();
		float t = 0.0f;
		if(rollEfectTimer_ <= 1.2f)
		{
			t = rollEfectTimer_ / 1.2f;
		}
		else
		{
			t = 0.9f - (rollEfectTimer_ - 1.2f) / 0.9f;
			t = EaseFixed::InQuint(t);
		}
		if (justRoll_)
		{
			PostEffectManager::GetInstance()->GetEffect<VignetteEffect>("Vignette")->SetPower(t);
			PostEffectManager::GetInstance()->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((screenOffset_ + Vector2(1.0f, 1.0f)) / 2.0f);
			PostEffectManager::GetInstance()->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.025f * t);
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