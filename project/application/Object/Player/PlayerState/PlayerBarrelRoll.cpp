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
    justRoll_ = true;
	if (justRoll_)
	{
		auto* pem = PostEffectManager::GetInstance();
		pem->SetEffectEnabled("Vignette", true);
		pem->GetEffect<VignetteEffect>("Vignette")->SetPower(0.0f);
		pem->SetEffectEnabled("RadialBlur", true);
		pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.0f);
		// 中心は画面座標系（-1..1）→ [0..1] に変換して渡す想定
		pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter(screenOffset_);
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
		auto* pem = PostEffectManager::GetInstance();
		pem->SetEffectEnabled("Vignette", false);
		pem->SetEffectEnabled("RadialBlur", false);
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
    if (rollEfectTimer_ <= 2.1f)
    {
        rollEfectTimer_ += Timer::GetInstance()->GetRawDeltaTime();

        float t = (rollEfectTimer_ <= 1.2f)
            ? (rollEfectTimer_ / 1.2f)
            : (0.9f - (rollEfectTimer_ - 1.2f) / 0.9f);

        t = (rollEfectTimer_ <= 1.2f) ? t : EaseFixed::InQuint(t);

        if (justRoll_)
        {
            auto* pem = PostEffectManager::GetInstance();
            pem->GetEffect<VignetteEffect>("Vignette")->SetPower(t);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((screenOffset_ + Vector2(1.0f, 1.0f)) * 0.5f);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.025f * t);
        }
    }

    // 画面オフセットのクランプ
    ClampOffset();

    // ===== ここが重要：ローカル平行移動を直接セット =====
    // 親がカメラなので、+Z(またはエンジン規約の前方)に playerDepthFromCamera_ 分だけ前進
    worldTransform_.translation_ = {
        screenOffset_.x * xRange,
        screenOffset_.y * yRange,
        playerDepthFromCamera_
    };

    // 回転はローカル軸で行う（Move() と同様の関数を使う）
    //   - roll は Z 軸（前方）回り
    //   - movePitch は X 軸回り
    // 親の向きは親子合成で自動的に乗るので、ここではローカルだけ。
    RotationOffsetLocal(); // あなたの前回差し替え版（Roll→Pitch の順など）

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