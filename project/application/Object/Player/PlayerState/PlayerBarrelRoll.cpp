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

using namespace TYEngine::OffScreen;
using namespace TYEngine::Utility;

void Player::InitBarrelRoll()
{
    // SE再生
    int h = GameAudio::GetInstance()->Play("roll", false, SoundCategory::SE);
	
	// ロール開始位置を記録
    barrelRoll_.startRollPos = screenOffset_;
    barrelRoll_.rollEffectTimer = 0.0f;
    
    // ラジアルブラーエフェクトの有効化と初期化
    auto* pem = PostEffectManager::GetInstance();
    pem->SetEffectEnabled("RadialBlur", true);
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(0.0f);
    // 中心は画面座標系（-1..1）→ [0..1] に変換して渡す想定
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter(screenOffset_);
    pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetNumSamples(barrelRoll_.blurSamples);

	
	// ジャスト回避時の演出設定
	if (barrelRoll_.justRoll)
	{
		pem->SetEffectEnabled("Vignette", true);
		pem->GetEffect<VignetteEffect>("Vignette")->SetPower(0.0f);
        GameAudio::GetInstance()->Pitch(h, barrelRoll_.audioPitch);
	}
    
    // 入力方向がある場合はその方向へ、なければランダム方向へロール
    if (Length(movement_.inputDir) != 0)
    {
        movement_.rollDir = movement_.inputDir;
    }
    else
    {
        movement_.rollDir = Normalize(Vector2(Random::GetInstance()->Float(-1,1), Random::GetInstance()->Float(-1, 1)));
    }
    
    // ロール中は当たり判定を無効化
    collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));
}

void Player::UpdateBarrelRoll()
{
	BarrelRoll();
    GameAudio::GetInstance()->Pitch(BGMHandle_, Timer::GetInstance()->GetTimeScale());
}

void Player::ExitBarrelRoll()
{
    // エフェクトの無効化
    auto* pem = PostEffectManager::GetInstance();
    pem->SetEffectEnabled("RadialBlur", false);
	if (barrelRoll_.justRoll)
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
        stateMachine_.ChangeState(PlayerState::BARREL_ROLL);
		
		// ジャスト回避判定
        barrelRoll_.justRoll = false;
		if(barrelRoll_.isJust)
		{
            barrelRoll_.justRoll = true;
			// バレットタイム演出トリガー
			BulletTimeController::GetInstance()->Trigger(
                barrelRoll_.slowScale, 
                barrelRoll_.enterDur, 
                barrelRoll_.holdDur, 
                barrelRoll_.exitDur,
				EaseFixed::InQuart, 
                EaseFixed::OutQuart);
		}
	}
}

// magic
void Player::BarrelRoll()
{
    // 入力方向に応じて左右（または上下）ロール
    if (movement_.rollDir.x != 0.0f)
    {
        (movement_.rollDir.x < 0.0f) ? LeftRoll(movement_.rollDir) : RightRoll(movement_.rollDir);
    }
    else if (movement_.rollDir.y != 0.0f)
    {
        (movement_.rollDir.y < 0.0f) ? LeftRoll(movement_.rollDir) : RightRoll(movement_.rollDir);
    }

    // エフェクト更新処理
    if (barrelRoll_.rollEffectTimer <= barrelRoll_.wholeEffectTime)
    {
        barrelRoll_.rollEffectTimer += Timer::GetInstance()->GetRawDeltaTime();

        // タイムラインによるエフェクト強度の制御（前半フェードイン、後半フェードアウト）
        float t = barrelRoll_.GetTimelineT();

        t = (barrelRoll_.rollEffectTimer <= barrelRoll_.firstHalfTime) ? t : EaseFixed::InQuint(t);

        auto* pem = PostEffectManager::GetInstance();
        if (barrelRoll_.justRoll)
        {
            // ジャスト回避時の特殊演出（ビネット効果追加）
            pem->GetEffect<VignetteEffect>("Vignette")->SetPower(t);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((Vector2(screenOffset_.x, -screenOffset_.y) + Vector2(1.0f, 1.0f)) * 0.5f);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(barrelRoll_.blurWidth * t);
        }
        else
        {
            // 通常ロール演出
            t = barrelRoll_.GetRollT();

            t = (barrelRoll_.rollEffectTimer <= barrelRoll_.secondHalfTime) ? t : EaseFixed::InQuint(t);

            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetCenter((Vector2(screenOffset_.x, -screenOffset_.y) + Vector2(1.0f, 1.0f)) * 0.5f);
            pem->GetEffect<RadialBlurEffect>("RadialBlur")->SetBlurWidth(barrelRoll_.blurWidth * t);
        }
    }

    // 画面オフセットのクランプ
    ClampOffset();

    worldTransform_.SetTranslation({
        screenOffset_.x * movement_.xRange,
        screenOffset_.y * movement_.yRange,
        movement_.playerDepthFromCamera
        });

    RotationOffsetLocal();

    // 終了判定
    if (stateMachine_.GetStateElapsedTime() >= barrelRoll_.rollTime)
    {
        stateMachine_.ChangeState(PlayerState::ROUTE);
    }
}



void Player::LeftRoll(const Vector2& dir)
{
    // ロール目標位置へイージング移動
    barrelRoll_.goalRollPos = barrelRoll_.startRollPos + dir * barrelRoll_.rollRange;
    float t = std::clamp(stateMachine_.GetStateElapsedTime() / barrelRoll_.rollTime, 0.0f, 1.0f);
    screenOffset_ = Lerp(barrelRoll_.startRollPos, barrelRoll_.goalRollPos, EaseFixed::OutBack(t));
    
    // 機体を回転させる
    movement_.roll = Lerp(0.0f, barrelRoll_.leftRoll, EaseFixed::OutBack(t));  // ローカルZ回転
}

void Player::RightRoll(const Vector2& dir)
{
    barrelRoll_.goalRollPos = barrelRoll_.startRollPos + dir * barrelRoll_.rollRange;
    float t = std::clamp(stateMachine_.GetStateElapsedTime() / barrelRoll_.rollTime, 0.0f, 1.0f);
    screenOffset_ = Lerp(barrelRoll_.startRollPos, barrelRoll_.goalRollPos, EaseFixed::OutBack(t));
    movement_.roll = Lerp(0.0f, barrelRoll_.rightRoll, EaseFixed::OutBack(t)); // ローカルZ回転
}