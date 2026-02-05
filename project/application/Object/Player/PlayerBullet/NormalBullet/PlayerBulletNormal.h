#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"

#include "../PBulletCollider.h"

/// <summary>
/// 通常弾の状態定数。
/// </summary>
enum class NormalBulletState
{
    LINEAR,           ///< 直進状態
    AFTER_COLLISION, ///< 衝突後（着弾エフェクト等）
};

/// <summary>
/// プレイヤーの通常弾クラス。
/// 直進挙動と衝突後の挙動をステートマシンで管理する。
/// </summary>
class PlayerBulletNormal :
    public BaseBullet, TYEngine::Utility::StateMachine<PlayerBulletNormal, NormalBulletState>
{
public: // 関数テーブル
    static const std::vector<StateFunctionSet>& GetStateTable();

public:
	PlayerBulletNormal();
    ~PlayerBulletNormal();
    void Init()override;
    void Update()override;
    void Draw()override;

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


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override
	{
		switch (state) 
		{
		case State::LINEAR: return "LINEAR";
		case State::AFTER_COLLISION: return "AFTER_COLLISION";
		default: return "Unknown";
		}
	}

	// 直線移動
	// LinearState.cpp
	void InitLinear();
	void UpdateLinear();
	void ExitLinear();

	// 衝突後処理
	// AfterCollisionState.cpp
	void InitAfterCollision();
	void UpdateAfterCollision();
	void ExitAfterCollision();
#pragma endregion
};

