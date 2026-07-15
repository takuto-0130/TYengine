#pragma once
#include "../../../engine/Framework/SceneBase/Transition/StateMachineTransition.h"
#include "State.h"
#include "Object3d.h"
#include <memory>
#include "struct.h"
#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

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

class BlockFadeTransitionStateIdle;
class BlockFadeTransitionStateEntering;
class BlockFadeTransitionStateHold;
class BlockFadeTransitionStateExiting;

class BlockFadeTransition : public StateMachineTransition<BlockFadeTransition>
{
	friend class BlockFadeTransitionStateIdle;
	friend class BlockFadeTransitionStateEntering;
	friend class BlockFadeTransitionStateHold;
	friend class BlockFadeTransitionStateExiting;
// 状態定義テーブル等のマクロ削除

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
	float lastElapsed_ = 0.0f;

	std::unique_ptr<TYEngine::Utility::JsonManager> jsonManager_;
	float playerScale_ = 2.0f;
	TYEngine::Utility::Vector2 playerStartPos_ = { 128.0f, 360.0f };
	TYEngine::Utility::Vector2 playerEndPos_ = { 1800.0f, 360.0f };
	float tileSpeedFactor_ = 4.0f;
};

// --- 状態クラスの定義 ---
class BlockFadeTransitionStateIdle : public TYEngine::Utility::State<TransitionStage, BlockFadeTransition>
{
public:
	using State::State;
	void Init(BlockFadeTransition& owner) override;
	void Update(BlockFadeTransition& owner, float deltaTime) override;
	void Exit(BlockFadeTransition& owner) override;
};

class BlockFadeTransitionStateEntering : public TYEngine::Utility::State<TransitionStage, BlockFadeTransition>
{
public:
	using State::State;
	void Init(BlockFadeTransition& owner) override;
	void Update(BlockFadeTransition& owner, float deltaTime) override;
	void Exit(BlockFadeTransition& owner) override;
};

class BlockFadeTransitionStateHold : public TYEngine::Utility::State<TransitionStage, BlockFadeTransition>
{
public:
	using State::State;
	void Init(BlockFadeTransition& owner) override;
	void Update(BlockFadeTransition& owner, float deltaTime) override;
	void Exit(BlockFadeTransition& owner) override;
};

class BlockFadeTransitionStateExiting : public TYEngine::Utility::State<TransitionStage, BlockFadeTransition>
{
public:
	using State::State;
	void Init(BlockFadeTransition& owner) override;
	void Update(BlockFadeTransition& owner, float deltaTime) override;
	void Exit(BlockFadeTransition& owner) override;
};

