#pragma once
#include "BaseObject.h"
#include "../ColliderTypeID/ColliderTypeID.h"


/// <summary>
/// ゲーム内キャラクター（プレイヤー・敵など）の基底クラス。
/// BaseObject を継承し、体力の概念や死亡フラグ、共通の衝突処理を持つ。
/// </summary>
class BaseCharacter : public BaseObject
{
public:
	// 死んだかどうか
public:
	/// <summary>
	/// キャラクターが死亡しているかどうかを取得する。
	/// </summary>
	/// <returns>死亡していれば true。</returns>
	virtual bool IsDead() const { return isDead_; }

	/// <summary>
	/// 衝突時のコールバック関数。
	/// デフォルトでは死亡フラグを true にする。
	/// </summary>
	virtual void OnCollision() { isDead_ = true; }

protected:
	/// <summary>死亡状態フラグ。</summary>
	bool isDead_ = false;
};