#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "PlayerCollider.h"
#include "StateMachine.h"


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
    public BaseCharacter, StateMachine<Player, PlayerState>
{
public: // 関数テーブル
    static const std::vector<StateFunctionSet>& GetStateTable();

public:
	~Player();
    void Init()override;
    void Update()override;
    void Draw()override;

	void SetCamera(Camera* camera) { camera_ = camera; }

private:
	void Move();
	void ClampOffset();

	Vector3 ConvertScreenOffsetToWorld(const Vector2& offset);
	void RotationOffset();

private:
	Input* input_ = nullptr;
	Camera* camera_ = nullptr;
	Vector2 screenOffset_ = {}; // カメラ基準のスクリーン内オフセット（例：[-1, 1]）
	std::unique_ptr<PlayerCollider> collider_;

	float scale_ = 0.1f;

	float playerDepthFromCamera_ = 4.0f;

	Vector2 defaultSpeed_ = { 1.0f,1.0f };
	Vector2 speed_ = defaultSpeed_;

	float xRange = 1.4f; // 横移動の最大幅（画面内の物理スケール）
	float yRange = 0.74f; // 縦移動の最大高さ

	float deltaTime_ = 1.0f / 60.0f;

private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override {
		switch (state) {
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

	// 通常行動
	void InitBoost();
	void UpdateBoost();
	void ExitBoost();

	// 通常行動
	void InitBarrelRoll();
	void UpdateBarrelRoll();
	void ExitBarrelRoll();

	// 通常行動
	void InitTakeDamage();
	void UpdateTakeDamage();
	void ExitTakeDamage();

	// 通常行動
	void InitDead();
	void UpdateDead();
	void ExitDead();
#pragma endregion
};

