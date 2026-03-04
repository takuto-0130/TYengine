#pragma once
#include "../StateMachineTransition.h"
#include "Object3d.h"
#include <memory>

namespace TYEngine::Graphics
{
	class Sprite;
}

struct BlockFadeConfig
{
	int   cols = 32;
	int   rows = 18;
	float inSec = 1.5f;
	float holdSec = 1.5f;
	float outSec = 1.5f;
	float easePow = 1.0f;        // 1=linear, 2=quad...
	bool  leftToRight = true;   // falseで右→左

	// プロシージャル矢印形状（全フェーズで使用）
	int   bodyCols = 32;   // ★胴体の横幅（列数）。推奨：cols以上（例: 40）
	int   headCols = 16;   // 先端の“最大”突き出し（中央で最大）
	float headPow = 1.0f; // 先端のプロファイル（中央尖り度）

	// 後端の“＞”切れ込み（中央最大, 上下最小）
	int   tailCols = 0;    // テール切れ込みの最大量（中央でこれだけ削る）
	float tailPow = 1.0f; // テールプロファイル（大きいほど中央が深く尖る）
};

class BlockFadeTransition : public StateMachineTransition<BlockFadeTransition>
{
public:
	using StateFunctionSet = TYEngine::Utility::StateMachine<BlockFadeTransition, TransitionStage>::StateFunctionSet;
	// 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
	enum class Type
	{
		IDLE,
		FADE_IN,
		HOLD,
		FADE_OUT,
	};

	BlockFadeTransition(BlockFadeTransition::Type type, const BlockFadeConfig& cfg);


	void Init() override;
	void Draw() override;
	bool IsFinished() const override;


public:
	const BlockFadeConfig& GetConfig() const { return cfg_; }

private:
	static float saturate(float v) { return std::max<float>(0.0f, std::min<float>(1.0f, v)); }

	BlockFadeConfig cfg_;
	std::vector<TYEngine::Graphics::Sprite> sprites_;   // タイル＝スプライト
	std::vector<float> viewSprites_;

	std::unique_ptr<TYEngine::Graphics::Sprite> pSpr_;

	float baseW_ = 0.0f, baseH_ = 0.0f;

	float duration_ = 0.0f;
	bool finished_ = false;

private:
	// IDLE
	virtual void InitIdle();
	virtual void UpdateIdle();
	virtual void ExitIdle();

	// ENTERING
	virtual void InitEntering();
	virtual void UpdateEntering();
	virtual void ExitEntering();

	// HOLD
	virtual void InitHold();
	virtual void UpdateHold();
	virtual void ExitHold();

	// EXITING
	virtual void InitExiting();
	virtual void UpdateExiting();
	virtual void ExitExiting();
};

