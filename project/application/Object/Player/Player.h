#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "PlayerCollider.h"
#include "JustCollider.h"
#include "StateMachine.h"
#include "PlayerBullet/PlayerBulletType.h"
#include "PlayerBullet/PlayerBulletManager.h"
#include "Reticle/Reticle.h"
#include <numbers>


enum class PlayerState
{
	IDLE,
    ROOT,
    BOOST,
    BARREL_ROLL,
    TAKE_DAMAGE,
    DEAD,
};

class Input;
class Camera;

class Player :
    public BaseCharacter, public StateMachine<Player, PlayerState>
{
public: // 関数テーブル
    static const std::vector<StateFunctionSet>& GetStateTable();

public:
	~Player();
    void Init()override;
    void Update()override;
    void Draw()override;

	void SetCamera(Camera* camera) { camera_ = camera; }

	Object3d* GetObj() { return obj_.get(); }

	void OnJust() { isJust_ = true; }
	void OffJust() { isJust_ = false; }

	bool IsJust() { return isJust_; }

	void SetScreenOffset(const Vector2& offset) { screenOffset_ = offset; }

	void TakeDamage()
	{
		if (hitpoint_ > 1)
		{
			hitpoint_--;
		}
		else
		{
			OnCollision();
		}
	}

	int GetHP() { return hitpoint_; }

	void OnCollision() override;

private:
	void PostStateUpdate();

	void Attack();
	void Move();
	void ClampOffset();

	void RotationOffsetLocal();

	Vector3 ConvertScreenOffsetToWorld(const Vector2& offset);
	//void RotationOffset();


	// BarrelRoll
	void StartBarrelRoll();
	void BarrelRoll();
	void LeftRoll();
	void RightRoll();


	// Debug
	void DebugGUI();
	// test
	void TestReticleInit();
	void TestReticleUpdate();
	void TestReticleDraw();

private:
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;
	Vector2 screenOffset_{}; // カメラ基準のスクリーン内オフセット（例：[-1, 1]）
	std::unique_ptr<PlayerCollider> collider_;
	std::unique_ptr<JustCollider> justCollider_;

	float colliderScale_ = 0.2f;
	float playerDepthFromCamera_ = 4.0f;
	float xRange = 16.0f * 0.09f; // 横移動の最大幅（画面内の物理スケール）
	float yRange = 9.0f * 0.085f; // 縦移動の最大高さ

	float defaultSpeed_ = 0.3f;
	Vector2 speed_{ defaultSpeed_, defaultSpeed_ * (yRange / xRange) };
	Vector2 inputDir_{};

	float deltaTime_ = 1.0f / 60.0f;

	// hp
	int hitpoint_ = 5;


	// 姿勢
	// Yaw（Y軸回転）
	float yaw = 0.0f;
	// Pitch（上下回転）
	float pitch = 0.0f;
	// Roll（横傾き）
	float roll = 0.0f;

	// Pitch（上下回転）
	float movePitch = 0.0f;

	// バレルロール
	float rollTime_ = 0.6f;
	float rollRange_ = 0.3f;
	float leftRoll_ = 2.0f * std::numbers::pi_v<float>;
	float rightRoll_ = -2.0f * std::numbers::pi_v<float>;
	Vector2 startRollPos_{};
	Vector2 goalRollPos_{};

	float rollEfectTimer_ = 0.0f;

	bool isJust_ = false;
	bool justRoll_ = false;
	float justScale_ = 3.0f;


	// 弾関連
	std::unique_ptr<PlayerBulletManager> bulletManager_;
	PlayerBulletType currentBulletType_ = PlayerBulletType::NORMAL;
	float bulletCoolTime_ = 0.3f;
	float bulletTimer_ = 0.0f;

	std::unique_ptr<Reticle> reticle_;


	// test
	std::unique_ptr<Object3d> reticleObj_;
	WorldTransform reticleWT_;

	int contrailIndex_;


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override 
	{
		switch (state) 
		{
		case State::IDLE: return "IDLE";
		case State::ROOT: return "ROOT";
		case State::BOOST: return "BOOST";
		case State::BARREL_ROLL: return "BARREL_ROLL";
		case State::TAKE_DAMAGE: return "TAKE_DAMAGE";
		case State::DEAD: return "DEAD";
		default: return "Unknown";
		}
	}

	// 待機状態
	void InitIdle();
	void UpdateIdle();
	void ExitIdle();

	// 通常行動
	void InitRoot();
	void UpdateRoot();
	void ExitRoot();

	// 加速
	void InitBoost();
	void UpdateBoost();
	void ExitBoost();

	// 回避
	void InitBarrelRoll();
	void UpdateBarrelRoll();
	void ExitBarrelRoll();

	// 被弾
	void InitTakeDamage();
	void UpdateTakeDamage();
	void ExitTakeDamage();

	// 死亡
	void InitDead();
	void UpdateDead();
	void ExitDead();
#pragma endregion
};

