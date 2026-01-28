#pragma once
#include "struct.h"

class EnemyBulletManager;

/// 攻撃行動の抽象インターフェース
class IAttackStrategy
{
public:
    virtual ~IAttackStrategy() = default;
    /**
     * @brief 攻撃を実行する
     * @param origin 発射位置（敵の座標）
     * @param target 目標位置（プレイヤー座標など）
     * @param manager 弾管理マネージャ
     */
    virtual void Attack(const Vector3& origin, const Vector3& target, EnemyBulletManager* manager) = 0;
};
