#pragma once
#include "Collider.h"
#include "struct.h"

namespace TYEngine {
namespace Utility {


/// <summary>
/// 球状コライダークラス
/// </summary>
class SphereCollider :
    public Collider
{
public: // メンバ関数
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="typeID">種類ID。</param>
    /// <param name="center">中心座標。</param>
    /// <param name="radius">半径。</param>
    SphereCollider(uint32_t typeID, const Vector3& center, float radius)
        : Collider(typeID), sphere_(Sphere{ center,radius }) {}

    // getter
    /// <summary>形状タイプを取得する。</summary>
    ColliderShape GetShapeType() const override { return ColliderShape::SPHERE; }
    /// <summary>中心座標を取得する。</summary>
    std::optional<Vector3> GetCenter() const override { return sphere_.center; }
    /// <summary>半径を取得する。</summary>
    float GetRadius() const { return sphere_.radius; }
    /// <summary>球情報を取得する。</summary>
    Sphere GetSphere() const { return sphere_; }

    /// <summary>
    /// 座標を更新する。
    /// </summary>
    /// <param name="pos">新しい中心座標。</param>
    void Update(const Vector3& pos) override { sphere_.center = pos; }

private: // メンバ変数
    Sphere sphere_;
};

} // namespace Utility
} // namespace TYEngine

