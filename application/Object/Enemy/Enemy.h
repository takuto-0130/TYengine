#pragma once
#include "BaseObject.h"
#include "IParticleRenderer.h"

class Enemy :
    public BaseObject
{
public:
	~Enemy() override;

	void Init() override;

	void Update() override;

	void Draw() override;

	Vector3 GetWorldPosition() const;

	bool IsDead() const { return isDead_; }

	void SetRotate(const Vector3& rota) { worldTransform_.rotation_ = rota; }

	void IsCollision() { isDead_ = true; }

	void SetPos(Vector3 pos) { 
		worldTransform_.translation_ = pos;
		worldTransform_.TransferMatrix();
	}

	void Pop();

private:
	bool isDead_ = false;

	IParticleRenderer::Emitter emitter;

	float popTimer_ = 0;
	const float kPopTime_ = 1.0f;

	const float deltaTime_ = 1.0f / 60.0f;

	Vector3 defaultScale_ = { 0.3f, 0.3f, 0.3f };

	const Vector3 ZeroScale = {};
};

float easeOutBounce(float x);
