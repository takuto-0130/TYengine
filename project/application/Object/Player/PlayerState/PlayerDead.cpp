#include "../Player.h"
#include "Effect/ParticleManager.h"
#include "../../../AppSystem/Audio/GameAudio.h"
#include "Timer.h"
#include "Ease.h"

using namespace TYEngine;
using namespace Utility;
using namespace Graphics;
using namespace Effect;

void PlayerStateDead::Init(Player& owner)
{
	// 死亡時音声再生
	GameAudio::GetInstance()->Play("gekiha", false, SoundCategory::SE);

	// パーティクル再生
	IParticleRenderer::Emitter e;
	e.transform.translate = owner.GetWorldPosition();
	e.count = owner.destroyEffect_.count;
	e.frequency = owner.destroyEffect_.frequency;
	e.transform.scale = owner.destroyEffect_.scale;
	ParticleManager::GetInstance()->SetEmitter(4, e);

	ParticleManager::GetInstance()->TriggerEmit(4, true);

	// 当たり判定を消す
	owner.collider_->SetTypeID(static_cast<uint32_t>(ColliderTypeID::NONE));

	// 落下パラメータの初期化
	owner.deadMotion_.fallSpeedY = -1.0f; // 少し上にフワッと浮いてから落ちる演出
}

void PlayerStateDead::Update(Player& owner, float deltaTime)
{
	(void)deltaTime;
	// キリモミ回転
	owner.movement_.roll += owner.deadMotion_.spinSpeed * owner.deltaTime_;

	// 機体を少し下に傾ける
	owner.movement_.movePitch = Lerp(owner.movement_.movePitch, owner.deadMotion_.targetPitch, 3.0f * owner.deltaTime_);

	// 落下処理
	owner.deadMotion_.fallSpeedY += owner.deadMotion_.gravity * owner.deltaTime_;
	owner.screenOffset_.y -= owner.deadMotion_.fallSpeedY * owner.deltaTime_;

	// 画面奥へ遠ざかる（Z軸）
	owner.movement_.playerDepthFromCamera += owner.deadMotion_.depthSpeed * owner.deltaTime_;

	// トランスフォームへの反映
	owner.worldTransform_.SetTranslation({
		owner.screenOffset_.x * owner.movement_.xRange,
		owner.screenOffset_.y * owner.movement_.yRange,
		owner.movement_.playerDepthFromCamera
		});

	// 6. 完全に画面下（または奥）へ消えたら処理を止める
	if (owner.screenOffset_.y < -2.5f) // 画面外に十分出たかを判定
	{
		owner.isDead_ = true;
	}
}

void PlayerStateDead::Exit(Player& owner)
{
	(void)owner;
}