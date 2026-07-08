#pragma once
#include "struct.h"

class PlayerBulletManager;

class Player;

// 敵の攻撃
namespace PlayerAttack
{
    /// <summary>
    /// 敵の攻撃行動（ストラテジーパターン）の基底インターフェース。
    /// </summary>
    class IPAttackStrategy
    {
    public:
        virtual ~IPAttackStrategy() = default;

        /// <summary>
        /// 汎用更新処理。
        /// </summary>
        /// <param name="p">プレイヤークラスのインスタンス。</param>
        virtual void Update(Player* p) = 0;

        /// <summary>
        /// 攻撃を実行する。
        /// </summary>
        /// <param name="origin">発射位置（敵の座標）。</param>
        /// <param name="target">目標位置（プレイヤー座標など）。</param>
        /// <param name="manager">弾管理マネージャ。</param>
        //virtual void Attack(const TYEngine::Utility::Vector3& origin, const TYEngine::Utility::Vector3& target, PlayerBulletManager* manager) = 0;
    };
}

