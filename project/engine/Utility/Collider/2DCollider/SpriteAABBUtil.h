#pragma once
#include "Sprite.h"
#include "Collision2D.h"
#include <cmath>

inline AABB2D ComputeSpriteAABB_NoRotation(Sprite& sprite)
{
    Vector2 pos = sprite.GetPosition();
    Vector2 size = sprite.GetSize();
    Vector2 anchor = sprite.GetAnchorPoint();

    float left = pos.x - size.x * anchor.x;
    float top = pos.y - size.y * anchor.y;
    float right = left + size.x;
    float bottom = top + size.y;

    AABB2D box;
    box.min = { left,  top };
    box.max = { right, bottom };
    return box;
}

inline AABB2D ComputeSpriteAABB_WithRotation(Sprite& sprite)
{
    Vector2 pos = sprite.GetPosition();
    Vector2 size = sprite.GetSize();
    Vector2 anchor = sprite.GetAnchorPoint();
    float   rot = sprite.GetRotation(); // ラジアン

    float left = -size.x * anchor.x;
    float right = left + size.x;
    float top = -size.y * anchor.y;
    float bottom = top + size.y;

    Vector2 local[4] = {
        { left,  top    },
        { right, top    },
        { left,  bottom },
        { right, bottom }
    };

    float c = std::cosf(rot);
    float s = std::sinf(rot);

    AABB2D box;
    box.min = { (std::numeric_limits<float>::max)(),  (std::numeric_limits<float>::max)() };
    box.max = { -(std::numeric_limits<float>::max)(), -(std::numeric_limits<float>::max)() };

    for (int i = 0; i < 4; ++i)
    {
        float wx = pos.x + local[i].x * c - local[i].y * s;
        float wy = pos.y + local[i].x * s + local[i].y * c;

        box.min.x = std::min<float>(box.min.x, wx);
        box.min.y = std::min<float>(box.min.y, wy);
        box.max.x = std::max<float>(box.max.x, wx);
        box.max.y = std::max<float>(box.max.y, wy);
    }
    return box;
}
