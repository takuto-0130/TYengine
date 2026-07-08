#pragma once
#include "IEnemyEventListener.h"
#include "../../Combo/ComboManager.h"
#include "../../Score/ScoreManager.h"

/// <summary>
/// 敵死亡時のイベントハンドラ。
/// コンボカウントの加算とスコアの加算を連携して行う。
/// </summary>
class ComboAndScoreHandler : public IEnemyEventListener
{
public:
    /// <summary>
    /// コンストラクタ。
    /// </summary>
    /// <param name="combo">コンボマネージャのポインタ。</param>
    /// <param name="score">スコアマネージャのポインタ。</param>
    ComboAndScoreHandler(HitStreakManager* combo, ScoreManager* score) : combo_(combo), score_(score) {}

    void OnEnemyDied([[maybe_unused]]Enemy* enemy) override
    {
        if (combo_)
        {
            combo_->OnEnemyDefeated();
            if (score_) score_->AddScore(1.0f + (float(combo_->GetComboCount()) * 0.2f));
        }
    }

private:
    /// <summary>コンボマネージャへの参照。</summary>
    HitStreakManager* combo_;
    /// <summary>スコアマネージャへの参照。</summary>
    ScoreManager* score_;
};