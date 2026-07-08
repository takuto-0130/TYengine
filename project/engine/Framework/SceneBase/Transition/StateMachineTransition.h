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