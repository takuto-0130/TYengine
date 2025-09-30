#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "EnemyCollider.h"
#include "StateMachine.h"
#include "Ease.h"
#include "IParticleRenderer.h"
#include "../../AppSystem/EventListener/EnemyEvent/IEnemyEventListener .h"
#include <iostream>
#include <random>

class EnemyBulletManager;

enum class EnemyState
{
	PRE_ENTER,	// 画面外・未表示（NDCの外）
	ENTERING,	// 画面外→画面内へ（イージングで移動、（もしかしたら透明→不透明？））
	ACTIVE,		// 敵アクティブ
	EXITING,	// 画面内→画面外へ（一定距離 / 時間で発火）
	DESPAWNED,	// 破棄（スコア扱いは設定で切替）
};

class Enemy :
    public BaseCharacter, StateMachine<Enemy, EnemyState>
{
public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
	~Enemy() override;

	void Init() override;

	void Update() override;

	void UpdateTransform();

	void Draw() override;

	void Pop();

	void OnCollision() override;

public:
	void SetEventListener(IEnemyEventListener* listener) { listener_ = listener; }

	void SetEnemyBulletManager(EnemyBulletManager* bulletManager) { bulletManager_ = bulletManager; }

	void SetAndApplyPos(Vector3 pos) 
	{
		worldTransform_.translation_ = pos;
		worldTransform_.TransferMatrix();
	}

	void SetTargetPos(Vector3 pos) { targetPos_ = pos; }

private:
	void IsShot();

	void Rotate();

private:
	std::unique_ptr<EnemyCollider> collider_;

	IParticleRenderer::Emitter emitter;

	float popTimer_ = 0;
	const float kPopTime_ = 1.0f;


	float kBulletCoolTime_ = 2.0f;
	float bulletTimer_ = 0.0f;


	float deltaTime_ = 1.0f / 60.0f;

	Vector3 defaultScale_ = { 0.3f, 0.3f, 0.3f };

	Vector3 upScale_ = { 0.45f, 0.45f, 0.45f };

	const Vector3 ZeroScale = {};

	Vector3 targetPos_ = {};


	IEnemyEventListener* listener_ = nullptr;

	EnemyBulletManager* bulletManager_;  // ポインタで保持

	std::random_device rd;


private: // シーン内のState関連関数
#pragma region // State関連関数
	// 列挙名を文字列化（ImGui表示用）
	std::string GetStateName(State state) const override
	{
		switch (state)
		{
		case State::PRE_ENTER: return "PRE_ENTER";
		case State::ENTERING: return "ENTERING";
		case State::ACTIVE: return "ACTIVE";
		case State::EXITING: return "EXITING";
		case State::DESPAWNED: return "DESPAWNED";
		default: return "Unknown";
		}
	}

	// 待機状態
	void InitPreEnter() {};
	void UpdatePreEnter() {};
	void ExitPreEnter() {};

	// 通常行動
	void InitEntering() {};
	void UpdateEntering() 
	{
		float t = GetStateElapsedTime() / kPopTime_;
		if (t <= 1.0f)
		{
			worldTransform_.colliderScale_ = Lerp(ZeroScale, defaultScale_, EaseFixed::InOutBounce(t));
		}
		else
		{
			ChangeState(EnemyState::ACTIVE);
		}
	};
	void ExitEntering() { worldTransform_.colliderScale_ = defaultScale_; }

	// 加速
	void InitActive() {}
	void UpdateActive() 
	{
		if (bulletTimer_ > 0.0f)
		{
			bulletTimer_ -= deltaTime_;
			if (bulletTimer_ >= 1.5f)
			{
				float t = bulletTimer_;
				if (t < 1.5f)
				{
					t = 1.5f;
				}
				worldTransform_.colliderScale_ = Lerp(defaultScale_, upScale_, EaseFixed::InOutBounce(t - 1.5f));
			}
		}
		else if (bulletTimer_ <= 0.0f)
		{
			IsShot();
		}
	}
	void ExitActive() {}

	// 回避
	void InitExiting() {}
	void UpdateExiting() {}
	void ExitExiting() {}

	// 被弾
	void InitDespawned() {}
	void UpdateDespawned() {}
	void ExitDespawned() {}
#pragma endregion
};
