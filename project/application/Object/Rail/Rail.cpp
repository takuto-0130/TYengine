#include "Rail.h"

using namespace TYEngine::Utility;
using namespace TYEngine::Graphics;

Rail::~Rail() 
{
}

void Rail::Init()
{
	// レール可視化用のオブジェクト生成（デバッグ用・エディタ用）
	obj_ = std::make_unique<Object3d>();
	obj_->Initialize();
	obj_->SetModel("cube.obj");
	
	// トランスフォーム
	worldTransform_.Initialize();
	worldTransform_.SetScale({ 0.06f, 0.03f, 0.4f });
}

void Rail::Update() 
{
	worldTransform_.Update();
}

void Rail::Draw() 
{
	obj_->Draw(worldTransform_);
}
