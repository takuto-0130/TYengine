#pragma once

// 当たり判定のタイプ
/// <summary>
/// コライダーのタイプID定義。
/// 衝突判定のフィルタリングや識別に使用する。
/// </summary>
enum class ColliderTypeID 
{
    NONE,       ///< なし/未設定
    PLAYER,     ///< プレイヤー本体
    P_BULLET,   ///< プレイヤーの弾
    JUST_AREA,  ///< ジャスト回避判定領域
    ENEMY,      ///< 敵本体
    E_BULLET,   ///< 敵の弾
    RETICLE,    ///< レティクル（照準）
};
