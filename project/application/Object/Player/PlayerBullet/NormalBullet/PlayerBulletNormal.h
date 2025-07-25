#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"

#include "../PBulletCollider.h"

enum class NormalBulletState
{
    LINER,
    AFTER_COLLISION,
};

class PlayerBulletNormal :
    public BaseBullet, StateMachine<PlayerBulletNormal, NormalBulletState>
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
	void Move();
	void RotationDirection();

private:
    std::unique_ptr<PBulletCollider> collider_;

	float deltaTime_ = 1.0f / 60.0f;


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override
	{
		switch (state) 
		{
		case State::LINER: return "LINER";
		case State::AFTER_COLLISION: return "AFTER_COLLISION";
		default: return "Unknown";
		}
	}

	// 直線移動
	// NormalBulletLiner.cpp
	void InitLiner();
	void UpdateLiner();
	void ExitLiner();

	// 衝突後処理
	// NormalBulletAfterCollision.cpp
	void InitAfterCollision();
	void UpdateAfterCollision();
	void ExitAfterCollision();
#pragma endregion
};

