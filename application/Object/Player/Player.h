#pragma once
#include "../BaseCharacter/BaseCharacter.h"
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

class Player :
    public BaseCharacter, StateMachine<Player, PlayerState>
{
public: // 関数テーブル
    static const std::vector<StateFunctionSet>& GetStateTable();

public:
    void Init()override;
    void Update()override;
    void Draw()override;


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

