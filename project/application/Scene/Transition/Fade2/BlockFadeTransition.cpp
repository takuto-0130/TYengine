#include "BlockFadeTransition.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "../TransitionManager.h"
#include "SpriteBasis.h"
#include "Timer.h"
#include <algorithm>

using namespace TYEngine::Utility;
using namespace TYEngine::Core;
using namespace TYEngine::Graphics;

#define BLOCKFADE_STATE_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(BlockFadeTransition, stateEnum, funcName)

const std::vector<StateMachine<BlockFadeTransition, TransitionStage>::StateFunctionSet>& BlockFadeTransition::GetStateTable()
{
	using enum TransitionStage;
	static const std::vector<StateFunctionSet> stateTable = {
		BLOCKFADE_STATE_ENTRY(IDLE, Idle),
		BLOCKFADE_STATE_ENTRY(ENTERING, Entering),
		BLOCKFADE_STATE_ENTRY(HOLD, Hold),
		BLOCKFADE_STATE_ENTRY(EXITING, Exiting)
	};
	return stateTable;
}

BlockFadeTransition::BlockFadeTransition(BlockFadeTransition::Type type, const BlockFadeConfig& cfg)
{
    cfg_ = cfg;
    stateMachine_.RegisterFromDefaultTable(this);


	switch (type)
	{
	case Type::IDLE:
        stateMachine_.ChangeState(TransitionStage::IDLE);
		break;
	case Type::FADE_IN:
        stateMachine_.ChangeState(TransitionStage::ENTERING);
		break;
	case Type::HOLD:
        stateMachine_.ChangeState(TransitionStage::HOLD);
		break;
	case Type::FADE_OUT:
        stateMachine_.ChangeState(TransitionStage::EXITING);
		break;
	}
}

void BlockFadeTransition::Init()
{
	baseW_ = WindowsApp::kClientWidth; baseH_ = WindowsApp::kClientHeight;

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Transition01.png");

	// タイル数ぶん Sprite を確保（各タイル=1スプライト）
	sprites_.clear();
	sprites_.resize(cfg_.cols * cfg_.rows);
	viewSprites_.clear();
	viewSprites_.resize(cfg_.cols * cfg_.rows);
	std::fill(viewSprites_.begin(), viewSprites_.end(), 0.0f);

	const float colW = baseW_ / cfg_.cols;
	const float rowH = baseH_ / cfg_.rows;

	for (int y = 0; y < cfg_.rows; ++y)
	{
		for (int x = 0; x < cfg_.cols; ++x)
		{
			auto& sp = sprites_[y * cfg_.cols + x];
			sp.Initialize("Resources/Texture/Transition01.png");
			sp.SetAnchorPoint({ 0.5f,0.5f });
			sp.SetColor({ 1,1,1,1 });

			// 1280x720基準で固定配置（スケーリングしない）
			float px = x * colW;
			float py = y * rowH;
			sp.SetPosition({ px + (colW / 2.0f), py + (rowH / 2.0f) });
			sp.SetSize({ colW * 0, rowH * 0 });
			sp.SetTextureSize({ colW, rowH });
			sp.SetTextureLeftTop({ px, py });
			sp.Update(); // 初期行列だけ作っておく（毎フレーム変更しない）
		}
	}
}

void BlockFadeTransition::Draw()
{
    if (stateMachine_.GetCurrentState() == TransitionStage::IDLE) return;

    const int cols = std::max<int>(1, cfg_.cols);
    const int rows = std::max<int>(1, cfg_.rows);

    auto physX = [&](int x)->int { return cfg_.leftToRight ? x : (cols - 1 - x); };

    // 進行度
    float t = 0.0f;
    if (stateMachine_.GetCurrentState() == TransitionStage::ENTERING) t = saturate(stateMachine_.GetStateElapsedTime() / cfg_.inSec);
    else if (stateMachine_.GetCurrentState() == TransitionStage::HOLD) t = 1.0f;
    else if (stateMachine_.GetCurrentState() == TransitionStage::EXITING) t = 1.0f - saturate(stateMachine_.GetStateElapsedTime() / cfg_.outSec);
    t = std::powf(t, cfg_.easePow);

    // 胴体左端の基準シフト（列単位）
    const float startIn = float(-(cfg_.cols + cfg_.headCols));   // 完全オフスクリーンから
    float shiftF = 0.0f;
    if (stateMachine_.GetCurrentState() == TransitionStage::ENTERING)        shiftF = std::lerp(startIn, 0.0f, t);
    else if (stateMachine_.GetCurrentState() == TransitionStage::HOLD)     shiftF = 0.0f;
    else if (stateMachine_.GetCurrentState() == TransitionStage::EXITING)  shiftF = std::lerp(0.0f, float(cols + cfg_.headCols), 1.0f - t);

    int shiftCols = int(std::floor(shiftF));

    // 行ごとに head(先端) を計算
    const float mid = (rows - 1) * 0.5f;

    for (int y = 0; y < rows; ++y)
    {
        float ny = std::abs((float(y) - mid) / std::max<float>(1.0f, mid)); // 中央0～端1

        // 先端（中央ほど前へ）
        float headY = cfg_.headCols * std::powf(1.0f - ny, cfg_.headPow);

        // ★後端（中央ほど深く削る）
        float tailY = cfg_.tailCols * std::powf(1.0f - ny, cfg_.tailPow);

        // 行yで塗る範囲（画面内だけ）
        int xMin = shiftCols + int(std::round(tailY));                // ← ここで“＞”の切れ込み
        int xMax = shiftCols + cfg_.bodyCols - 1 + int(std::round(headY));

        int drawMin = std::max<int>(0, xMin);
        int drawMax = std::min<int>(cols - 1, xMax);
        if (drawMin <= drawMax)
        {
            for (int x = drawMin; x <= drawMax; ++x)
            {
                int px = physX(x);
                int idx = y * cols + px;

                viewSprites_[idx] += Timer::GetInstance()->GetDeltaTime() / (cfg_.holdSec / 4.0f);
                viewSprites_[idx] = std::min<float>(viewSprites_[idx], 1.0f);
                const float colW = baseW_ / cfg_.cols;
                const float rowH = baseH_ / cfg_.rows;
                sprites_[idx].SetSize({ colW * viewSprites_[idx], rowH * viewSprites_[idx] });
                sprites_[idx].Update();
                sprites_[idx].Draw();
            }
        }

        if (stateMachine_.GetCurrentState() == TransitionStage::EXITING)
        {
            if (drawMin <= drawMax)
            {
                for (int x = 0; x <= drawMin; ++x)
                {
                    int px = physX(x);
                    int idx = y * cols + px;

                    if (viewSprites_[idx] >= 1.0f)
                    {
                        viewSprites_[idx] -= Timer::GetInstance()->GetDeltaTime() / (cfg_.holdSec / 4.0f);
                        viewSprites_[idx] = std::max<float>(viewSprites_[idx], 0.0f);
                    }
                }
            }
            for (int x = 0; x < cols; ++x)
            {
                int px = physX(x);
                int idx = y * cols + px;

                if (viewSprites_[idx] > 0.0f)
                {
                    if (viewSprites_[idx] < 1.0f)
                    {
                        viewSprites_[idx] -= Timer::GetInstance()->GetDeltaTime() / (cfg_.holdSec / 4.0f);
                        viewSprites_[idx] = std::max<float>(viewSprites_[idx], 0.0f);
                    }
                    const float colW = baseW_ / cfg_.cols;
                    const float rowH = baseH_ / cfg_.rows;
                    sprites_[idx].SetSize({ colW * viewSprites_[idx], rowH * viewSprites_[idx] });
                    sprites_[idx].Update();
                    sprites_[idx].Draw();
                }
            }
        }

    }
}

bool BlockFadeTransition::IsFinished() const
{
	return finished_;
}



// IDLE
void BlockFadeTransition::InitIdle()
{

}
void BlockFadeTransition::UpdateIdle()
{

}
void BlockFadeTransition::ExitIdle()
{

}

// ENTERING
void BlockFadeTransition::InitEntering()
{
	std::fill(viewSprites_.begin(), viewSprites_.end(), 0.0f);
}
void BlockFadeTransition::UpdateEntering()
{
	// イージングを適用した進行度tを計算
	float t = std::powf(saturate(stateMachine_.GetStateElapsedTime() / cfg_.inSec), cfg_.easePow);
	
	// 左から右への波及計算
	float shiftF = std::lerp(float(-(cfg_.cols + cfg_.headCols + 0)), 0.0f, t);
	int   shiftCols = int(std::floor(shiftF));

	bool rightOK = (shiftCols >= (cfg_.cols - cfg_.bodyCols));

	// 画面全体が覆われたらHOLDへ
	if (rightOK)
	{
        stateMachine_.ChangeState(TransitionStage::HOLD);
	}
}
void BlockFadeTransition::ExitEntering()
{

}

// HOLD
void BlockFadeTransition::InitHold()
{
    std::fill(viewSprites_.begin(), viewSprites_.end(), 1.0f);
}
void BlockFadeTransition::UpdateHold()
{
	if (stateMachine_.GetStateElapsedTime() >= cfg_.holdSec)
	{
		finished_ = true;
	}
}
void BlockFadeTransition::ExitHold()
{

}

// EXITING
void BlockFadeTransition::InitExiting()
{
    std::fill(viewSprites_.begin(), viewSprites_.end(), 1.0f);
}
void BlockFadeTransition::UpdateExiting()
{
	if (stateMachine_.GetStateElapsedTime() >= cfg_.outSec + (cfg_.holdSec / 4.0f))
	{
		finished_ = true;
	}
}
void BlockFadeTransition::ExitExiting()
{

}