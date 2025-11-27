#pragma once
#include <vector>
#include "Sprite.h"
#include "Collision2D.h"
#include "SpriteAABBUtil.h"

class UISpriteColliderManager
{
public:
    // 初期登録（ポインタ配列を渡す）
    void SetSprites(const std::vector<Sprite*>& sprites)
    {
        sprites_ = sprites;
        aabbs_.resize(sprites_.size());
    }

    // 1つずつ追加したい場合用
    void AddSprite(Sprite* sprite)
    {
        sprites_.push_back(sprite);
        aabbs_.resize(sprites_.size());
    }

    void ClearSprites()
    {
        sprites_.clear();
        aabbs_.clear();
    }

    // 毎フレーム、UI スプライトの Update 後に呼ぶ
    void BuildAABBs(bool useRotation = false)
    {
        const size_t n = sprites_.size();
        if (aabbs_.size() != n)
        {
            aabbs_.resize(n);
        }

        for (size_t i = 0; i < n; ++i)
        {
            Sprite* s = sprites_[i];
            if (!s)
            {
                aabbs_[i] = { {0,0}, {0,0} };
                continue;
            }

            aabbs_[i] = useRotation
                ? ComputeSpriteAABB_WithRotation(*s)
                : ComputeSpriteAABB_NoRotation(*s);
        }
    }

    const std::vector<AABB2D>& GetAABBs() const { return aabbs_; }

private:
    std::vector<Sprite*> sprites_;
    std::vector<AABB2D>  aabbs_;
};
