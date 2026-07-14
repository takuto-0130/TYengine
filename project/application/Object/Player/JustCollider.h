#pragma once
#include "Sphere/SphereCollider.h"

class Player;
/// <summary>
/// ジャスト回避判定用のコライダー。
/// プレイヤー等の本体コライダーよりも少し大きく設定し、かすり判定（グレイズ）を行うために使用する。
/// </summary>
class JustCollider :
    public TYEngine::Utility::SphereCollider
{
public:
    /// <summary>コンストラクタ。</summary>
    JustCollider(uint32_t typeID, const TYEngine::Utility::Vector3& center, float radius, Player* player)
        : SphereCollider(typeID, center, radius), player_(player) {
    }

    /// <summary>衝突中の処理。</summary>
    void OnCollisionStay([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;
    /// <summary>衝突終了時の処理。</summary>
    void OnCollisionExit([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;

private:
    /// <summary>所有者（プレイヤー）。</summary>
    Player* player_ = nullptr;
};

