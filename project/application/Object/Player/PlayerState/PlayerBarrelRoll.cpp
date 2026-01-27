#include "../Player.h"
#include "input.h"
#include "Timer.h"
#include "Ease.h"
#include "Random.h"
#include "BulletTimeController.h"
#include "PostEffectManager.h"
#include "VignetteEffect.h"
#include "RadialBlurEffect.h"
#include "../../../AppSystem/Audio/GameAudio.h"

void Player::InitBarrelRoll()
{
    GameAudio::GetInstance()->Play("roll", false, SoundCategory::SE);
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
    if (Length(inputDir_) != 0)
    {
        rollDir_ = inputDir_;
    }
    else
    {
        rollDir_ = Normalize(Vector2(Random::GetInstance()->Float(-1,1), Random::GetInstance()->Float(-1, 1)));
    }
    collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));
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
    collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::PLAYER));
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
    if (rollDir_.x != 0.0f)
    {
        (rollDir_.x < 0.0f) ? LeftRoll(rollDir_) : RightRoll(rollDir_);
    }
    else if (rollDir_.y != 0.0f)
    {
        (rollDir_.y < 0.0f) ? LeftRoll(rollDir_) : RightRoll(rollDir_);
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
    if (GetStateElapsedTime() >= rollTime_)
    {
        ChangeState(PlayerState::ROOT);
    }
}



void Player::LeftRoll(const Vector2& dir)
{
    goalRollPos_ = startRollPos_ + dir * rollRange_;
    float t = std::clamp(GetStateElapsedTime() / rollTime_, 0.0f, 1.0f);
    screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
    roll = Lerp(0.0f, leftRoll_, EaseFixed::OutBack(t));  // ローカルZ回転
}

void Player::RightRoll(const Vector2& dir)
{
    goalRollPos_ = startRollPos_ + dir * rollRange_;
    float t = std::clamp(GetStateElapsedTime() / rollTime_, 0.0f, 1.0f);
    screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
    roll = Lerp(0.0f, rightRoll_, EaseFixed::OutBack(t)); // ローカルZ回転
}