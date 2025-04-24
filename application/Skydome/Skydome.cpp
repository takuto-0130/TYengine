#include "Skydome.h"
#include "assert.h"

void Skydome::Initialize() {
	skydome = std::make_unique<Object3d>();
	skydome->Initialize();
	skydome->SetModel("skydome.obj");
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 500.0f, 500.0f, 500.0f };
}

void Skydome::Update() {
	worldTransform_.TransferMatrix();
}

void Skydome::Draw() {
	skydome->Draw(worldTransform_);
}