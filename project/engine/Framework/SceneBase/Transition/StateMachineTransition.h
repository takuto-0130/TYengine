#pragma once
#include "ITransition.h"
#include "StateMachine.h"

/// <summary>画面遷移アニメーションの実行フェード段階を表す列挙型。</summary>
enum class TransitionStage
{
	IDLE,       // 待機中/処理なし
	ENTERING,   // 遷移開始（シーンへ入る）
    HOLD,       // 保持
	EXITING,	// 遷移終了（シーンを出る）
};

/// <summary>
/// ステートマシン（StateMachine）を組み込んで段階的な画面遷移（イン/ホールド/アウト）を実現する遷移基底テンプレートクラス。
/// </summary>
/// <typeparam name="Class">派生画面遷移クラス。</typeparam>
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