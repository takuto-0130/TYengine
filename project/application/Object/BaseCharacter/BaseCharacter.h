#pragma once
#include "BaseObject.h"
#include "../ColliderTypeID/ColliderTypeID.h"


class BaseCharacter : public BaseObject
{
public:
	// 死んだかどうか
	virtual bool IsDead() const { return isDead_; }

	// 衝突時処理
	virtual void OnCollision() { isDead_ = true; }

protected:
	// 死亡判定フラグ
	bool isDead_ = false;
};