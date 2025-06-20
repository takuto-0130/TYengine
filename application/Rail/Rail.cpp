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
	worldTransform_.scale_ = { 0.3f, 0.03f, 0.3f };
}

void Rail::Update() 
{
	worldTransform_.TransferMatrix();
}

void Rail::Draw() 
{
	obj_->Draw(worldTransform_);
}
