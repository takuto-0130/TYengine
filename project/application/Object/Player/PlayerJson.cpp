#include "Player.h"

using namespace TYEngine;
using namespace Utility;
using namespace Debugger;

void Player::JMInit()
{
	jm_.Load("PlayerConfig.json", true, &err_);
#ifdef _DEBUG
	Log(err_);
#endif // _DEBUG

	// 値反映
	// 数値を適用
	colliderScale_ = jm_.Get<float>("colliderScale");

	// 移動関連
	movement_.Load(jm_);

	// HP設定
	status_.Load(jm_);

	// バレルロール
	barrelRoll_.Load(jm_);

	// ロックオン
	lockOn_.Load(jm_);

	// 弾関連
	bullets_.Load(jm_);

	// エフェクト
	jetEffect_.Load(jm_);
	destroyEffect_.Load(jm_);

	// 死亡時モーション
	deadMotion_.Load(jm_);

	// 被弾設定
	takeDamage_.Load(jm_);
}

void Player::DebugJMApply()
{
#ifdef _DEBUG
	// 移動関連
	movement_.Load(jm_);

	// HP設定


	// バレルロール
	barrelRoll_.Load(jm_);

	// ロックオン
	lockOn_.Load(jm_);

	// 弾関連
	bullets_.Load(jm_);

	jetEffect_.Load(jm_);

	destroyEffect_.Load(jm_);

	deadMotion_.Load(jm_);

	takeDamage_.Load(jm_);
#endif // _DEBUG
}