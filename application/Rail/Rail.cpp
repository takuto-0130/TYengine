#include "Rail.h"
#include "mathFunc.h"
#include "operatorOverload.h"



Rail::~Rail() 
{
}


void Rail::Initialize(Vector3 position)
{
	rail_ = std::make_unique<Object3d>();
	rail_->Initialize();
	rail_->SetModel("cube.obj");
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = { 0.3f, 0.03f, 0.3f };
}

void Rail::Update() 
{
	worldTransform_.TransferMatrix();
}

void Rail::Draw() 
{
	rail_->Draw(worldTransform_);
}

Vector3 Rail::GetWorldPosition() const 
{
	Vector3 worldPos;
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}
