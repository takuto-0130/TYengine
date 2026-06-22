#pragma once
#include "../StateMachineTransition.h"
#include "State.h"
#include <memory>

namespace TYEngine::Graphics
{
	class Sprite;
}

class FadeTransitionStateIdle;
class FadeTransitionStateEntering;
class FadeTransitionStateExiting;

/// <summary>
/// フェードイン・フェードアウトを行うトランジションクラス。
/// ステートマシンを使用して状態を管理する。
/// </summary>
class FadeTransition : public StateMachineTransition<FadeTransition>
{
	friend class FadeTransitionStateIdle;
	friend class FadeTransitionStateEntering;
	friend class FadeTransitionStateExiting;
// 状態定義テーブル等のマクロ削除

public:
	/// <summary>
	/// フェードの種類の定義。
	/// </summary>
	enum class Type
	{
		IDLE,		///< 待機
		FADE_IN,	///< フェードイン（黒 -> 透明）
		FADE_OUT,	///< フェードアウト（透明 -> 黒）
	};

	/// <summary>
	/// コンストラクタ。
	/// </summary>
	/// <param name="type">フェードの種類。</param>
	/// <param name="duration">フェードにかける時間（秒）。</param>
	FadeTransition(FadeTransition::Type type, float duration);


	/// <summary>初期化処理。</summary>
	void Init() override;
	/// <summary>描画処理。</summary>
	void Draw() override;
	/// <summary>完了判定。</summary>
	bool IsFinished() const override;

private:
	/// <summary>フェード用スプライト（全画面矩形）。</summary>
	std::unique_ptr<TYEngine::Graphics::Sprite> sprites_;
	/// <summary>フェード時間。</summary>
	float duration_ = 0.0f;
	/// <summary>完了フラグ。</summary>
	bool finished_ = false;
};

// --- 状態クラスの定義 ---
class FadeTransitionStateIdle : public TYEngine::Utility::State<TransitionStage, FadeTransition>
{
public:
	using State::State;
	void Init(FadeTransition& owner) override;
	void Update(FadeTransition& owner, float deltaTime) override;
	void Exit(FadeTransition& owner) override;
};

class FadeTransitionStateEntering : public TYEngine::Utility::State<TransitionStage, FadeTransition>
{
public:
	using State::State;
	void Init(FadeTransition& owner) override;
	void Update(FadeTransition& owner, float deltaTime) override;
	void Exit(FadeTransition& owner) override;
};

class FadeTransitionStateExiting : public TYEngine::Utility::State<TransitionStage, FadeTransition>
{
public:
	using State::State;
	void Init(FadeTransition& owner) override;
	void Update(FadeTransition& owner, float deltaTime) override;
	void Exit(FadeTransition& owner) override;
};

