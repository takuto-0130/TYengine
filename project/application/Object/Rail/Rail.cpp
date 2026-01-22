#include "Rail.h"

Rail::~Rail() 
{
}

void Rail::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	worldTransform_.Initialize();
	worldTransform_.colliderScale_ = { 0.06f, 0.03f, 0.4f };
}

void Rail::Update() 
{
	worldTransform_.TransferMatrix();
}

void Rail::Draw() 
{
	obj_->Draw(worldTransform_);
}
