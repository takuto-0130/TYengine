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
	rollEffectTimer_ = 0.0f;
    auto* pem = PostEffectManager::GetInstance();
    pem->SetEffectEnabled("RadialBlur", true);
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.0f);
    // 中心は画面座標系（-1..1）→ [0..1] に変換して渡す想定
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter(screenOffset_);
	if (justRoll_)
	{
		pem->SetEffectEnabled("Vignette", true);
		pem->GetEffect<VignetteEffect>("Vignette")->SetPower(0.0f);
	}
}

void Player::UpdateBarrelRoll()
{
	BarrelRoll();
}

void Player::ExitBarrelRoll()
{
    auto* pem = PostEffectManager::GetInstance();
    pem->SetEffectEnabled("RadialBlur", false);
	if (justRoll_)
	{
		pem->SetEffectEnabled("Vignette", false);
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
    // 入力方向に応じて左右（または上下）ロール
    if (inputDir_.x != 0.0f)
    {
        (inputDir_.x < 0.0f) ? LeftRoll() : RightRoll();
    }
    else if (inputDir_.y != 0.0f)
    {
        (inputDir_.y < 0.0f) ? LeftRoll() : RightRoll();
    }

    // エフェクト
    if (rollEffectTimer_ <= 2.1f)
    {
        rollEffectTimer_ += Timer::GetInstance()->GetRawDeltaTime();

        float t = (rollEffectTimer_ <= 1.2f)
            ? (rollEffectTimer_ / 1.2f)
            : (0.9f - (rollEffectTimer_ - 1.2f) / 0.9f);

        t = (rollEffectTimer_ <= 1.2f) ? t : EaseFixed::InQuint(t);

        auto* pem = PostEffectManager::GetInstance();
        if (justRoll_)
        {
            pem->GetEffect<VignetteEffect>("Vignette")->SetPower(t);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((Vector2(screenOffset_.x, -screenOffset_.y) + Vector2(1.0f, 1.0f)) * 0.5f);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.025f * t);
        }
        else
        {
            t = (rollEffectTimer_ <= 0.9f)
                ? (rollEffectTimer_ / 0.9f)
                : (0.9f - (rollEffectTimer_ - 0.9f) / 0.9f);

            t = (rollEffectTimer_ <= 0.9f) ? t : EaseFixed::InQuint(t);

            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((Vector2(screenOffset_.x, -screenOffset_.y) + Vector2(1.0f, 1.0f)) * 0.5f);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.025f * t);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetNumSamples(3);
        }
    }

    // 画面オフセットのクランプ
    ClampOffset();

    worldTransform_.translation_ = {
        screenOffset_.x * xRange,
        screenOffset_.y * yRange,
        playerDepthFromCamera_
    };

    RotationOffsetLocal();

    // 終了判定
    if (GetStateElapsedTime() >= rollTime_ || Length(inputDir_) == 0.0f)
    {
        ChangeState(PlayerState::ROOT);
    }
}



void Player::LeftRoll()
{
    goalRollPos_ = startRollPos_ + inputDir_ * rollRange_;
    float t = std::clamp(GetStateElapsedTime() / rollTime_, 0.0f, 1.0f);
    screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
    roll = Lerp(0.0f, leftRoll_, EaseFixed::OutBack(t));  // ローカルZ回転
}

void Player::RightRoll()
{
    goalRollPos_ = startRollPos_ + inputDir_ * rollRange_;
    float t = std::clamp(GetStateElapsedTime() / rollTime_, 0.0f, 1.0f);
    screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
    roll = Lerp(0.0f, rightRoll_, EaseFixed::OutBack(t)); // ローカルZ回転
}