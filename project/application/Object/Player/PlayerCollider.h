#pragma once
#include "Sphere/SphereCollider.h"

class Player;
/// <summary>
/// プレイヤー用のコライダー（球形状）。
/// </summary>
class PlayerCollider :
    public TYEngine::Utility::SphereCollider
{
public:
    /// <summary>コンストラクタ。</summary>
    PlayerCollider(uint32_t typeID, const TYEngine::Utility::Vector3& center, float radius, Player* player)
        : SphereCollider(typeID, center, radius), player_(player) {
    }

    /// <summary>衝突開始時の処理。</summary>
    void OnCollisionEnter([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;
    /// <summary>衝突中の処理。</summary>
    void OnCollisionStay([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;
    /// <summary>衝突終了時の処理。</summary>
    void OnCollisionExit([[maybe_unused]] TYEngine::Utility::Collider& other, [[maybe_unused]] const TYEngine::Utility::CollisionInfo& info) override;

private:
    /// <summary>所有者（プレイヤー）。</summary>
    Player* player_ = nullptr;
};

