#include "Enemy.h"

Enemy::~Enemy()
{
}

void Enemy::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("unitSphere.obj");
	obj_->SetColor({ 1.0f,0.0f,0.0f,1.0f });
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 0.3f, 0.3f, 0.3f };
}

void Enemy::Update()
{
	worldTransform_.TransferMatrix();
}

void Enemy::Draw()
{
	obj_->Draw(worldTransform_);
}

Vector3 Enemy::GetWorldPosition() const
{
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
