#include "Skydome.h"
#include "assert.h"


void Skydome::Init()
{
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("skydome.obj");
	worldTransform_.Initialize();
	worldTransform_.colliderScale_ = { 500.0f, 500.0f, 500.0f };
	obj_->SetIsLighting(false);
}

void Skydome::Update() 
{
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() 
{
	obj_->Draw(worldTransform_);
}