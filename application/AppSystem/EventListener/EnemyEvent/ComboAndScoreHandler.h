#pragma once
#include "IEnemyEventListener .h"
#include "../../Combo/ComboManager.h"
#include "../../Score/ScoreManager.h"

class ComboAndScoreHandler : public IEnemyEventListener {
public:
    ComboAndScoreHandler(ComboManager* combo, ScoreManager* score)
        : combo_(combo), score_(score) {
    }

    void OnEnemyDied([[maybe_unused]]Enemy* enemy) override {
        if (combo_)
        {
            combo_->OnEnemyDefeated();
            if (score_) score_->AddScore(float(combo_->GetComboCount()));
        }
    }

private:
    ComboManager* combo_;
    ScoreManager* score_;
};