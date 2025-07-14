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

private:
	Camera* camera_ = nullptr;
	Input* input_ = nullptr;
	std::unique_ptr<ReticleCollider> collider_;
};

