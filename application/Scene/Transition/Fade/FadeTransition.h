#pragma once
#include "../StateMachineTransition.h"
#include <memory>

class Sprite;

class FadeTransition : public StateMachineTransition<FadeTransition>
{
public: // 関数テーブル
	static const std::vector<StateMachine<FadeTransition, TransitionStage>::StateFunctionSet>& GetStateTable();

public:
	enum class Type
	{
		IDLE,
		FADE_IN,
		FADE_OUT,
	};

	FadeTransition(FadeTransition::Type type, float duration);


	void Init() override;
	void Draw() override;
	bool IsFinished() const override;

private:
	// IDLE
	virtual void EnterIdle();
	virtual void UpdateIdle();
	virtual void ExitIdle();

	// ENTERING
	virtual void EnterEntering();
	virtual void UpdateEntering();
	virtual void ExitEntering();

	// EXITING
	virtual void EnterExiting();
	virtual void UpdateExiting();
	virtual void ExitExiting();

private:
	std::unique_ptr<Sprite> sprite_;
	float duration_ = 0.0f;
	bool finished_ = false;
};

