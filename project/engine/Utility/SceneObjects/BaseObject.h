#pragma once
#include "Object3d.h"
#include "WorldTransform.h"

// オブジェクトのベースクラス
class BaseObject
{
public:
	// デストラクタ
	virtual ~BaseObject() {};

	// 初期化
	virtual void Init() = 0;

	// 更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;

protected:
	// 3Dオブジェクト
	std::unique_ptr<Object3d> obj_;

	// ワールド行列
	WorldTransform worldTransform_;
};

