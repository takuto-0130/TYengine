#include "Player.h"

void Player::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 0.3f, 0.3f, 0.3f };
}

void Player::Update()
{
	UpdateState(1.0f / 60.0f);
	worldTransform_.TransferMatrix();
}

void Player::Draw()
{
}
