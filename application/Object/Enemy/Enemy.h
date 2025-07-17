#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "EnemyCollider.h"
#include "IParticleRenderer.h"
#include "../../AppSystem/EventListener/EnemyEvent/IEnemyEventListener .h"

class Enemy :
    public BaseCharacter
{
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

	void SetAndApplyPos(Vector3 pos) 
	{
		worldTransform_.translation_ = pos;
		worldTransform_.TransferMatrix();
	}

private:
	std::unique_ptr<EnemyCollider> collider_;

	IParticleRenderer::Emitter emitter;

	float popTimer_ = 0;
	const float kPopTime_ = 1.0f;

	const float deltaTime_ = 1.0f / 60.0f;

	Vector3 defaultScale_ = { 0.3f, 0.3f, 0.3f };

	const Vector3 ZeroScale = {};


	IEnemyEventListener* listener_ = nullptr;
};

float easeOutBounce(float x);
