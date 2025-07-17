#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "EnemyCollider.h"
#include "IParticleRenderer.h"

class IParticleRenderer;

class Enemy :
    public BaseCharacter
{
public:
	~Enemy() override;

	void Init() override;

	void Update() override;

	void UpdateTransform();

	void Draw() override;

	void SetAndApplyPos(Vector3 pos) { 
		worldTransform_.translation_ = pos;
		worldTransform_.TransferMatrix();
	}

	void Pop();

	void OnCollision() override
	{
		isDead_ = true;
		if (onDeath_) {
			onDeath_(this); // コンボ加算などを外部に通知
		}
	}

public:
	void SetOnDeathCallback(std::function<void(Enemy*)> callback) { onDeath_ = std::move(callback); }

private:
	std::unique_ptr<EnemyCollider> collider_;

	bool isDead_ = false;

	IParticleRenderer::Emitter emitter;

	float popTimer_ = 0;
	const float kPopTime_ = 1.0f;

	const float deltaTime_ = 1.0f / 60.0f;

	Vector3 defaultScale_ = { 0.3f, 0.3f, 0.3f };

	const Vector3 ZeroScale = {};


	// 外部からセット可能
	std::function<void(Enemy*)> onDeath_;
};

float easeOutBounce(float x);
