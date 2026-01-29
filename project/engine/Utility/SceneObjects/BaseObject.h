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

	// ワールド座標を取得
	virtual Vector3 GetWorldPosition()
	{
		Vector3 worldPos;
		worldPos.x = worldTransform_.GetMatWorld().m[3][0];
		worldPos.y = worldTransform_.GetMatWorld().m[3][1];
		worldPos.z = worldTransform_.GetMatWorld().m[3][2];
		return worldPos;
	}

	// ローカルスケールをセット
	virtual void SetScale(const Vector3& scale) { worldTransform_.SetScale(scale); }

	// ローカル回転をセット
	virtual void SetRotate(const Vector3& rota) { worldTransform_.SetRotate(rota); }

	// ローカル座標をセット
	virtual void SetTranslation(const Vector3& pos) { worldTransform_.SetTranslation(pos); }

protected:
	// 3Dオブジェクト
	std::unique_ptr<Object3d> obj_;

	// ワールド行列
	WorldTransform worldTransform_;
};

