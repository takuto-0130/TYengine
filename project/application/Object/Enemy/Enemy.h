#pragma once
#include "../BaseCharacter/BaseCharacter.h"
#include "EnemyCollider.h"
#include "IParticleRenderer.h"
#include "../../AppSystem/EventListener/EnemyEvent/IEnemyEventListener .h"
#include <iostream>
#include <random>

class EnemyBulletManager;
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
};
