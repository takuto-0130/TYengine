#include "LevelObject.h"

void LevelObject::Init()
{
	worldTransform_.Initialize();

	// 3Dオブジェクト生成とデータセット
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel(modelName_);
}

void LevelObject::Update()
{
	worldTransform_.Update();
}

void LevelObject::Draw()
{
	obj_->Draw(worldTransform_);
}
