#pragma once
#include "BaseObject.h"
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

private:
	bool isDead_ = false;
};

