#include "BlockFadeTransition.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "../../../engine/Framework/SceneBase/Transition/TransitionManager.h"
#include "SpriteBasis.h"
#include "Timer.h"
#include <algorithm>

using namespace TYEngine::Utility;
using namespace TYEngine::Core;
using namespace TYEngine::Graphics;

// マクロ・テーブルの削除

BlockFadeTransition::BlockFadeTransition(BlockFadeTransition::Type type, const BlockFadeConfig& cfg)
{
    cfg_ = cfg;
    stateMachine_.RegisterState<BlockFadeTransitionStateIdle>(TransitionStage::IDLE, "Idle");
    stateMachine_.RegisterState<BlockFadeTransitionStateEntering>(TransitionStage::ENTERING, "Entering");
    stateMachine_.RegisterState<BlockFadeTransitionStateHold>(TransitionStage::HOLD, "Hold");
    stateMachine_.RegisterState<BlockFadeTransitionStateExiting>(TransitionStage::EXITING, "Exiting");


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
	jsonManager_ = std::make_unique<TYEngine::Utility::JsonManager>();
	std::string err;
	jsonManager_->Load("TransitionConfig.json", true, &err);

	playerScale_ = jsonManager_->Get<float>("playerScale", 2.0f);
	playerStartPos_ = jsonManager_->Get<TYEngine::Utility::Vector2>("playerStartPos", { 128.0f, 360.0f });
	playerEndPos_ = jsonManager_->Get<TYEngine::Utility::Vector2>("playerEndPos", { 1800.0f, 360.0f });
	tileSpeedFactor_ = jsonManager_->Get<float>("tileSpeedFactor", 4.0f);

	lastElapsed_ = 0.0f;
	baseW_ = WindowsApp::kClientWidth; baseH_ = WindowsApp::kClientHeight;

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Transition01.png");
    TextureManager::GetInstance()->LoadTexture("Resources/Texture/Player.png");

	// タイル数ぶん Sprite を確保（各タイル=1スプライト）
	sprites_.clear();
	sprites_.resize(cfg_.cols * cfg_.rows);
	viewSprites_.clear();
	viewSprites_.resize(cfg_.cols * cfg_.rows);
	std::fill(viewSprites_.begin(), viewSprites_.end(), 0.0f);

    pSpr_ = std::make_unique<Sprite>();
    pSpr_->Initialize("Resources/Texture/Player.png");
    pSpr_->SetAnchorPoint({ 0.5f,0.5f });
    pSpr_->SetScale(playerScale_);

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
#ifdef _DEBUG
	playerScale_ = jsonManager_->Get<float>("playerScale", 2.0f);
	playerStartPos_ = jsonManager_->Get<TYEngine::Utility::Vector2>("playerStartPos", { 128.0f, 360.0f });
	playerEndPos_ = jsonManager_->Get<TYEngine::Utility::Vector2>("playerEndPos", { 1800.0f, 360.0f });
	tileSpeedFactor_ = jsonManager_->Get<float>("tileSpeedFactor", 4.0f);
#endif

    if (stateMachine_.GetCurrentState() == TransitionStage::IDLE) return;

    float currentElapsed = stateMachine_.GetStateElapsedTime();
    float dt = currentElapsed - lastElapsed_;
    if (dt < 0.0f) dt = currentElapsed;
    lastElapsed_ = currentElapsed;

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

                viewSprites_[idx] += dt / (cfg_.holdSec / tileSpeedFactor_);
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
                        viewSprites_[idx] -= dt / (cfg_.holdSec / tileSpeedFactor_);
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
                        viewSprites_[idx] -= dt / (cfg_.holdSec / tileSpeedFactor_);
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

        if (stateMachine_.GetCurrentState() == TransitionStage::ENTERING)
        {
            pSpr_->SetPosition(Lerp(playerStartPos_, playerEndPos_, t));
            pSpr_->Update();
            pSpr_->Draw();
        }
    }
}

bool BlockFadeTransition::IsFinished() const
{
	return finished_;
}



// --- 状態クラスのメソッド実装 ---
void BlockFadeTransitionStateIdle::Init(BlockFadeTransition&) {}
void BlockFadeTransitionStateIdle::Update(BlockFadeTransition&, float) {}
void BlockFadeTransitionStateIdle::Exit(BlockFadeTransition&) {}

void BlockFadeTransitionStateEntering::Init(BlockFadeTransition& owner)
{
	std::fill(owner.viewSprites_.begin(), owner.viewSprites_.end(), 0.0f);
}
void BlockFadeTransitionStateEntering::Update(BlockFadeTransition& owner, float)
{
	// イージングを適用した進行度tを計算
	float t = std::powf(owner.saturate(GetElapsed() / owner.cfg_.inSec), owner.cfg_.easePow);
	
	// 左から右への波及計算
	float shiftF = std::lerp(float(-(owner.cfg_.cols + owner.cfg_.headCols + 0)), 0.0f, t);
	int   shiftCols = int(std::floor(shiftF));

	bool rightOK = (shiftCols >= (owner.cfg_.cols - owner.cfg_.bodyCols));

	// 画面全体が覆われたらHOLDへ
	if (rightOK)
	{
		RequestStateChange(TransitionStage::HOLD);
	}
}
void BlockFadeTransitionStateEntering::Exit(BlockFadeTransition&) {}

void BlockFadeTransitionStateHold::Init(BlockFadeTransition& owner)
{
	std::fill(owner.viewSprites_.begin(), owner.viewSprites_.end(), 1.0f);
}
void BlockFadeTransitionStateHold::Update(BlockFadeTransition& owner, float)
{
	if (GetElapsed() >= owner.cfg_.holdSec)
	{
		owner.finished_ = true;
	}
}
void BlockFadeTransitionStateHold::Exit(BlockFadeTransition&) {}

void BlockFadeTransitionStateExiting::Init(BlockFadeTransition& owner)
{
	std::fill(owner.viewSprites_.begin(), owner.viewSprites_.end(), 1.0f);
}
void BlockFadeTransitionStateExiting::Update(BlockFadeTransition& owner, float)
{
	if (GetElapsed() >= owner.cfg_.outSec + (owner.cfg_.holdSec / 4.0f))
	{
		owner.finished_ = true;
	}
}
void BlockFadeTransitionStateExiting::Exit(BlockFadeTransition&) {}