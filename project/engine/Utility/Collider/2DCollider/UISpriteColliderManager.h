#pragma once
#include <vector>
#include "Sprite.h"
#include "Collision2D.h"
#include "SpriteAABBUtil.h"

/// <summary>
/// UI用スプライトのコライダー管理クラス。
/// 複数のスプライトのAABBを一括計算・管理する。
/// </summary>
class UISpriteColliderManager
{
public:
    /// <summary>
    /// 管理対象のスプライトリストを設定する。
    /// </summary>
    /// <param name="sprites">スプライトのリスト。</param>
    void SetSprites(const std::vector<Sprite*>& sprites)
    {
        sprites_ = sprites;
        aabbs_.resize(sprites_.size());
    }

    /// <summary>
    /// スプライトを単体で追加する。
    /// </summary>
    /// <param name="sprite">追加するスプライト。</param>
    void AddSprite(Sprite* sprite)
    {
        sprites_.push_back(sprite);
        aabbs_.resize(sprites_.size());
    }

    /// <summary>
    /// 登録済みのスプライトをクリアする。
    /// </summary>
    void ClearSprites()
    {
        sprites_.clear();
        aabbs_.clear();
    }

    /// <summary>
    /// 全スプライトのAABBを再計算する。
    /// 毎フレーム、スプライト更新後に呼び出す必要がある。
    /// </summary>
    /// <param name="useRotation">回転を考慮するかどうか。</param>
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

    /// <summary>
    /// 現在計算済みのAABBリストを取得する。
    /// </summary>
    const std::vector<AABB2D>& GetAABBs() const { return aabbs_; }

private:
    std::vector<Sprite*> sprites_;
    std::vector<AABB2D>  aabbs_;
};
