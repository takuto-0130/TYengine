#pragma once
#include "struct.h"

namespace TYEngine {
namespace Utility {


/// <summary>
/// 2Dの軸平行境界ボックス（Axis-Aligned Bounding Box）。
/// 左上座標(min)と右下座標(max)で定義される。
/// </summary>
struct AABB2D
{
    Vector2 min; ///< 左上座標
    Vector2 max; ///< 右下座標
};

/// <summary>
/// 2つのAABB2Dが交差しているか判定する。
/// </summary>
/// <param name="a">AABB A。</param>
/// <param name="b">AABB B。</param>
/// <returns>交差していれば true。</returns>
inline bool IntersectAABB(const AABB2D& a, const AABB2D& b)
{
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    return true;
}

} // namespace Utility
} // namespace TYEngine
