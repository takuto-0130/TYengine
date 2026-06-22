#pragma once
#include "ITransition.h"
#include "StateMachine.h"

enum class TransitionStage
{
	IDLE,       // 待機中/処理なし
	ENTERING,   // 遷移開始（シーンへ入る）
    HOLD,       // 保持
	EXITING,	// 遷移終了（シーンを出る）
};

// ステートマシン遷移テンプレート
// 'Class' = 継承先クラス（例：FadeTransition）
template<typename Class>
class StateMachineTransition : 
     public ITransition
{
public:
    void Update(float dt) override 
    {
        stateMachine_.UpdateState(*static_cast<Class*>(this), dt);
    }

protected:
    TYEngine::Utility::StateMachine<TransitionStage, Class> stateMachine_;
};

// 関数テーブル等の宣言例
/* 
.h
public:
	using StateFunctionSet = TYEngine::Utility::StateMachine<Class, TransitionStage>::StateFunctionSet;
	// 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

.cpp
Class()
{
    stateMachine_.RegisterFromDefaultTable(this);
}
*/