#pragma once
#include "struct.h"

struct AABB2D
{
    Vector2 min; // 左上
    Vector2 max; // 右下
};

inline bool IntersectAABB(const AABB2D& a, const AABB2D& b)
{
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    return true;
}
