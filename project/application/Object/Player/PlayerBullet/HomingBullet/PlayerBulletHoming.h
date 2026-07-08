#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"
#include "State.h"

#include "../PBulletCollider.h"
#include "Utils/Json/JsonManager.h"

// 前方宣言
class HomingBulletStateShot;
class HomingBulletStateAfterCollision;

/// <summary>
/// 通常弾の状態定数。
/// </summary>
enum class HomingBulletState
{
	SHOT,				///< 発射状態
	AFTER_COLLISION,	///< 衝突後（着弾エフェクト等）
};

class EnemyManager;

/// <summary>
/// プレイヤーの通常弾クラス。
/// 直進挙動と衝突後の挙動をステートマシンで管理する。
/// </summary>
class PlayerBulletHoming :
	public BaseBullet
{
	friend class HomingBulletStateShot;
	friend class HomingBulletStateAfterCollision;
public:
	using StateMachineType = TYEngine::Utility::StateMachine<HomingBulletState, PlayerBulletHoming>;

public:
	PlayerBulletHoming();
	~PlayerBulletHoming();
	void Init()override;
	void Update()override;
	void Draw()override;

	void SetJM(TYEngine::Utility::JsonManager* jm) { jm_ = jm; }

	void SetEnemyManager(EnemyManager* enemyMgr) { enemyMgr_ = enemyMgr; }

private:
	/// <summary>移動処理（直進）。</summary>
	void Move();
	/// <summary>進行方向に合わせた回転更新。</summary>
	void RotationDirection();

private:
	/// <summary>弾用コライダー。</summary>
	std::unique_ptr<PBulletCollider> collider_;

	/// <summary>固定タイムステップ（60FPS想定）。</summary>
	float deltaTime_ = 0.0f;

	float minHoming_ = 0.0f;
	float maxHoming_ = 0.0f;
	float homingTime_ = 0.0f;

	EnemyManager* enemyMgr_ = nullptr;

	/// <summary>JSONマネージャ。</summary>
	TYEngine::Utility::JsonManager* jm_ = nullptr;

	/// <summary>ステートマシーン。</summary>
	StateMachineType stateMachine_;

};

// --- 状態クラスの定義 ---
class HomingBulletStateShot : public TYEngine::Utility::State<HomingBulletState, PlayerBulletHoming>
{
public:
	using State::State;
	void Init(PlayerBulletHoming& owner) override;
	void Update(PlayerBulletHoming& owner, float deltaTime) override;
	void Exit(PlayerBulletHoming& owner) override;
};

class HomingBulletStateAfterCollision : public TYEngine::Utility::State<HomingBulletState, PlayerBulletHoming>
{
public:
	using State::State;
	void Init(PlayerBulletHoming& owner) override;
	void Update(PlayerBulletHoming& owner, float deltaTime) override;
	void Exit(PlayerBulletHoming& owner) override;
};

