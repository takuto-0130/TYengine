#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"

#include "../PBulletCollider.h"

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
	public BaseBullet, TYEngine::Utility::StateMachine<PlayerBulletHoming, HomingBulletState>
{
public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
	PlayerBulletHoming();
	~PlayerBulletHoming();
	void Init()override;
	void Update()override;
	void Draw()override;

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

	// <summary>ホーミング強度係数。</summary>
	float homingT_ = 0.0f;

	EnemyManager* enemyMgr_ = nullptr;

private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override
	{
		switch (state)
		{
		case State::SHOT: return "SHOT";
		case State::AFTER_COLLISION: return "AFTER_COLLISION";
		default: return "Unknown";
		}
	}

	// 直線移動
	// ShotState.cpp
	void InitShot();
	void UpdateShot();
	void ExitShot();

	// 衝突後処理
	// AfterCollisionState.cpp
	void InitAfterCollision();
	void UpdateAfterCollision();
	void ExitAfterCollision();
#pragma endregion
};

