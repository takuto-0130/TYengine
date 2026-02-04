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
    // SE再生
    GameAudio::GetInstance()->Play("roll", false, SoundCategory::SE);
	
	// ロール開始位置を記録
	startRollPos_ = screenOffset_;
	rollEffectTimer_ = 0.0f;
    
    // ラジアルブラーエフェクトの有効化と初期化
    auto* pem = PostEffectManager::GetInstance();
    pem->SetEffectEnabled("RadialBlur", true);
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.0f);
    // 中心は画面座標系（-1..1）→ [0..1] に変換して渡す想定
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter(screenOffset_);
	
	// ジャスト回避時の演出設定
	if (justRoll_)
	{
		pem->SetEffectEnabled("Vignette", true);
		pem->GetEffect<VignetteEffect>("Vignette")->SetPower(0.0f);
	}
    
    // 入力方向がある場合はその方向へ、なければランダム方向へロール
    if (Length(inputDir_) != 0)
    {
        rollDir_ = inputDir_;
    }
    else
    {
        rollDir_ = Normalize(Vector2(Random::GetInstance()->Float(-1,1), Random::GetInstance()->Float(-1, 1)));
    }
    
    // ロール中は当たり判定を無効化
    collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));
}

void Player::UpdateBarrelRoll()
{
	BarrelRoll();
}

void Player::ExitBarrelRoll()
{
    // エフェクトの無効化
    auto* pem = PostEffectManager::GetInstance();
    pem->SetEffectEnabled("RadialBlur", false);
	if (justRoll_)
	{
		pem->SetEffectEnabled("Vignette", false);
	}
    // 当たり判定をプレイヤー用に戻す
    collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::PLAYER));
}


// magic
void Player::StartBarrelRoll()
{
	// LShiftキーでバレルロール開始
	if(input_->TriggerKey(DIK_LSHIFT))
	{
		ChangeState(PlayerState::BARREL_ROLL);
		
		// ジャスト回避判定
		justRoll_ = false;
		if(isJust_)
		{
			justRoll_ = true;
			// バレットタイム演出トリガー
			BulletTimeController::GetInstance()->Trigger(
				0.05f, 0.0f, 1.0f, 0.8f,
				EaseFixed::InQuart, EaseFixed::OutQuart);
		}
	}
}

// magic
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

    // エフェクト更新処理
    if (rollEffectTimer_ <= 2.1f)
    {
        rollEffectTimer_ += Timer::GetInstance()->GetRawDeltaTime();

        // タイムラインによるエフェクト強度の制御（前半フェードイン、後半フェードアウト）
        float t = (rollEffectTimer_ <= 1.2f)
            ? (rollEffectTimer_ / 1.2f)
            : (0.9f - (rollEffectTimer_ - 1.2f) / 0.9f);

        t = (rollEffectTimer_ <= 1.2f) ? t : EaseFixed::InQuint(t);

        auto* pem = PostEffectManager::GetInstance();
        if (justRoll_)
        {
            // ジャスト回避時の特殊演出（ビネット効果追加）
            pem->GetEffect<VignetteEffect>("Vignette")->SetPower(t);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((Vector2(screenOffset_.x, -screenOffset_.y) + Vector2(1.0f, 1.0f)) * 0.5f);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.025f * t);
        }
        else
        {
            // 通常ロール演出
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

    worldTransform_.SetTranslation({
        screenOffset_.x * xRange,
        screenOffset_.y * yRange,
        playerDepthFromCamera_
        });

    RotationOffsetLocal();

    // 終了判定
    if (GetStateElapsedTime() >= rollTime_)
    {
        ChangeState(PlayerState::ROUTE);
    }
}



void Player::LeftRoll(const Vector2& dir)
{
    // ロール目標位置へイージング移動
    goalRollPos_ = startRollPos_ + dir * rollRange_;
    float t = std::clamp(GetStateElapsedTime() / rollTime_, 0.0f, 1.0f);
    screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
    
    // 機体を回転させる
    roll = Lerp(0.0f, leftRoll_, EaseFixed::OutBack(t));  // ローカルZ回転
}

void Player::RightRoll(const Vector2& dir)
{
    goalRollPos_ = startRollPos_ + dir * rollRange_;
    float t = std::clamp(GetStateElapsedTime() / rollTime_, 0.0f, 1.0f);
    screenOffset_ = Lerp(startRollPos_, goalRollPos_, EaseFixed::OutBack(t));
    roll = Lerp(0.0f, rightRoll_, EaseFixed::OutBack(t)); // ローカルZ回転
}