#pragma once
#include "ReticleCollider.h"
#include <memory>

class Camera;
class Input;
class Reticle
{
public:
	Reticle(Camera* camera) : camera_(camera) {}
	~Reticle();
	void Init();
	void Update();
	void Draw();

private:
	void ScreenToWorld();

public:
	void SetFrameDistance(float distance) { frameDistance_ == 0.0f || frameDistance_ >= distance ? frameDistance_ = distance : frameDistance_; }
	float GetTargetDistance() const { return targetDistance_; }
	Ray GetRay() const { return collider_->GetRay(); }

	Vector3 GetTarget() { return collider_->GetRay().origin + collider_->GetRay().diff * targetDistance_; }

private:
	Camera* camera_ = nullptr;
	Input* input_ = nullptr;
	std::unique_ptr<ReticleCollider> collider_;
	float targetDistance_ = 0.0f;
	float frameDistance_ = 0.0f;
	const float defaultDistance_ = 50.0f;
};

