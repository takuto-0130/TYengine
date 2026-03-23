#include "../Player.h"
#include "Effect/ParticleManager.h"
#include "../../../AppSystem/Audio/GameAudio.h"
#include "Timer.h"
#include "Ease.h"

using namespace TYEngine;
using namespace Utility;
using namespace Graphics;
using namespace Effect;

void Player::InitDead()
{
	// 死亡時音声再生
	GameAudio::GetInstance()->Play("gekiha", false, SoundCategory::SE);

	// パーティクル再生
	IParticleRenderer::Emitter e;
	e.transform.translate = GetWorldPosition();
	e.count = destroyEffect_.count;
	e.frequency = destroyEffect_.frequency;
	e.transform.scale = destroyEffect_.scale;
	ParticleManager::GetInstance()->SetEmitter(4, e);

	ParticleManager::GetInstance()->TriggerEmit(4, true);

	// 当たり判定を消す
	collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));

	// 落下パラメータの初期化
	deadMotion_.fallSpeedY = -1.0f; // 少し上にフワッと浮いてから落ちる演出
}

void Player::UpdateDead()
{
	// キリモミ回転
	movement_.roll += deadMotion_.spinSpeed * deltaTime_;

	// 機体を少し下に傾ける
	movement_.movePitch = Lerp(movement_.movePitch, deadMotion_.targetPitch, 3.0f * deltaTime_);

	// 落下処理
	deadMotion_.fallSpeedY += deadMotion_.gravity * deltaTime_;
	screenOffset_.y -= deadMotion_.fallSpeedY * deltaTime_;

	// 画面奥へ遠ざかる（Z軸）
	movement_.playerDepthFromCamera += deadMotion_.depthSpeed * deltaTime_;

	// トランスフォームへの反映
	worldTransform_.SetTranslation({
		screenOffset_.x * movement_.xRange,
		screenOffset_.y * movement_.yRange,
		movement_.playerDepthFromCamera
		});

	// 6. 完全に画面下（または奥）へ消えたら処理を止める
	if (screenOffset_.y < -2.5f) // 画面外に十分出たかを判定
	{
		isDead_ = true;
	}
}

void Player::ExitDead()
{

}