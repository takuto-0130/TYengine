#pragma once
#include "BaseObject.h"


class BaseCharacter : public BaseObject
{
public:
	// ワールド座標を取得
	virtual Vector3 GetWorldPosition() 
	{
		Vector3 worldPos;
		worldPos.x = worldTransform_.matWorld_.m[3][0];
		worldPos.y = worldTransform_.matWorld_.m[3][1];
		worldPos.z = worldTransform_.matWorld_.m[3][2];
		return worldPos;
	}

	// ローカルスケールをセット
	virtual void SetScale(const Vector3& scale) { worldTransform_.scale_ = scale; }

	// ローカル回転をセット
	virtual void SetRotate(const Vector3& rota) { worldTransform_.rotation_ = rota; }

	// ローカル座標をセット
	virtual void SetTranslation(Vector3 pos) { worldTransform_.translation_ = pos; }

	// 死んだかどうか
	virtual bool IsDead() const { return isDead_; }

	// 衝突時処理
	virtual void OnCollision() { isDead_ = true; }

private:
	// 死亡判定フラグ
	bool isDead_ = false;
};