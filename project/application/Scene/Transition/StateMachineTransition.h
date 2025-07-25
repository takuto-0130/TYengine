#pragma once
#include "ITransition.h"
#include "StateMachine.h"

enum class TransitionStage
{
	IDLE,       // 待機中・処理なし
	ENTERING,   // 遷移開始（シーンへ入る）
	EXITING,	// 遷移終了（シーンを出る）
};

// CRTP化したステートマシン遷移テンプレート
// 'Class' = 継承先クラス（例：FadeTransition）
template<typename Class>
class StateMachineTransition : public ITransition, public StateMachine<Class, TransitionStage>
{
public:
    void Update(float dt) override {
        this->UpdateState(dt);
    }

protected:
    std::string GetStateName(TransitionStage state) const override {
        switch (state) {
        case TransitionStage::IDLE: return "IDLE";
        case TransitionStage::ENTERING: return "ENTERING";
        case TransitionStage::EXITING: return "EXITING";
        default: return "Unknown";
        }
    }
};

// 関数テーブル等の宣言例
/* 
.h
static const std::vector<StateMachine<Class, TransitionStage>::StateFunctionSet>& GetStateTable();

.cpp
Class()
{
    this->template RegisterFromDefaultTable<Class>(this);
}
*/