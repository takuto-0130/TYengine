//// BlockFadeOverlay.h
//#pragma once
//#include "Sprite.h"
//#include "DirectXBasis.h"
//#include <vector>
//#include <string>
//#include <algorithm>
//#include <cmath>
//
//enum class FadePhase { Idle, FadeIn, Hold, FadeOut };
//
//struct BlockFadeConfig
//{
//    int   cols = 32;
//    int   rows = 18;
//    float inSec = 1.5f;
//    float holdSec = 1.5f;
//    float outSec = 1.5f;
//    float easePow = 1.0f;        // 1=linear, 2=quad...
//    bool  leftToRight = true;   // falseで右→左
//
//    // プロシージャル矢印形状（全フェーズで使用）
//    int   bodyCols = 32;   // ★胴体の横幅（列数）。推奨：cols以上（例: 40）
//    int   headCols = 16;   // 先端の“最大”突き出し（中央で最大）
//    float headPow = 1.0f; // 先端のプロファイル（中央尖り度）
//
//    // ★後端の“＞”切れ込み（中央最大, 上下最小）
//    int   tailCols = 0;    // テール切れ込みの最大量（中央でこれだけ削る）
//    float tailPow = 1.0f; // テールプロファイル（大きいほど中央が深く尖る）
//};
//
//// スプライト配列フェード
//class BlockFadeOverlay
//{
//public:
//    void Init(const std::string& white1x1TexPath, const BlockFadeConfig& cfg)
//    {
//        cfg_ = cfg;
//        baseW_ = 1280.0f; baseH_ = 720.0f;
//
//        // タイル数ぶん Sprite を確保（各タイル=1スプライト）
//        sprites_.clear();
//        sprites_.resize(cfg_.cols * cfg_.rows);
//        viewSprites_.clear();
//        viewSprites_.resize(cfg_.cols * cfg_.rows);
//        std::fill(viewSprites_.begin(), viewSprites_.end(), 0.0f);
//
//        const float colW = baseW_ / cfg_.cols;
//        const float rowH = baseH_ / cfg_.rows;
//
//        for (int y = 0; y < cfg_.rows; ++y)
//        {
//            for (int x = 0; x < cfg_.cols; ++x)
//            {
//                auto& sp = sprites_[y * cfg_.cols + x];
//                sp.Initialize(white1x1TexPath);
//                sp.SetAnchorPoint({ 0.5f,0.5f });
//                sp.SetColor({ 1,1,1,1 });
//
//                // 1280x720基準で固定配置（スケーリングしない）
//                float px = x * colW;
//                float py = y * rowH;
//                sp.SetPosition({ px + (colW / 2.0f), py + (rowH  / 2.0f)});
//                sp.SetSize({ colW * 0, rowH * 0 });
//                sp.SetTextureSize({ colW, rowH });
//                sp.SetTextureLeftTop({ px, py });
//                sp.Update(); // 初期行列だけ作っておく（毎フレーム変更しない）
//            }
//        }
//    }
//
//    // フェーズ制御
//    void StartFadeIn()
//    { 
//        phase_ = FadePhase::FadeIn;  
//        timer_ = 0;
//        std::fill(viewSprites_.begin(), viewSprites_.end(), 0.0f);
//    }
//    void StartHold() { phase_ = FadePhase::Hold;    timer_ = 0; }
//    void StartFadeOut() { phase_ = FadePhase::FadeOut; timer_ = 0; }
//    bool IsBusy() const { return phase_ != FadePhase::Idle; }
//    FadePhase Phase() const { return phase_; }
//
//    void Update(float dt)
//    {
//        if (phase_ == FadePhase::Idle) return;
//        dt_ = dt;
//        timer_ += dt_;
//
//        auto sat = [](float v) { return std::max<float>(0.0f, std::min<float>(1.0f, v)); };
//
//        if (phase_ == FadePhase::FadeIn)
//        {
//            float t = std::powf(sat(timer_ / cfg_.inSec), cfg_.easePow);
//            float shiftF = std::lerp(float(-(cfg_.cols + cfg_.headCols + 0)), 0.0f, t);
//            int   shiftCols = int(std::floor(shiftF));
//
//            bool rightOK = (shiftCols >= (cfg_.cols - cfg_.bodyCols));
//
//            if (rightOK)
//            {       // 胴体だけで全面覆った！
//                phase_ = FadePhase::Hold;
//                timer_ = 0.0f;
//            }
//        }
//        else if (phase_ == FadePhase::Hold)
//        {
//            if (timer_ >= cfg_.holdSec)
//            {
//                phase_ = FadePhase::FadeOut;
//                timer_ = 0.0f;
//            }
//        }
//        else if (phase_ == FadePhase::FadeOut)
//        {
//            if (timer_ >= cfg_.outSec + (cfg_.holdSec / 4.0f))
//            {
//                phase_ = FadePhase::Idle;
//                timer_ = 0.0f;
//            }
//        }
//    }
//
//    // フレームの一番最後（UI含む全描画の後）に呼ぶ
//    void Draw()
//    {
//        if (phase_ == FadePhase::Idle) return;
//
//        const int cols = std::max<int>(1, cfg_.cols);
//        const int rows = std::max<int>(1, cfg_.rows);
//
//        auto physX = [&](int x)->int { return cfg_.leftToRight ? x : (cols - 1 - x); };
//
//        // 進行度
//        float t = 0.0f;
//        if (phase_ == FadePhase::FadeIn) t = saturate(timer_ / cfg_.inSec);
//        else if (phase_ == FadePhase::Hold) t = 1.0f;
//        else if (phase_ == FadePhase::FadeOut) t = 1.0f - saturate(timer_ / cfg_.outSec);
//        t = std::powf(t, cfg_.easePow);
//
//        // 胴体左端の基準シフト（列単位）
//        const float startIn = float(-(cfg_.cols + cfg_.headCols));   // 完全オフスクリーンから
//        float shiftF = 0.0f;
//        if (phase_ == FadePhase::FadeIn)        shiftF = std::lerp(startIn, 0.0f, t);
//        else if (phase_ == FadePhase::Hold)     shiftF = 0.0f;
//        else if (phase_ == FadePhase::FadeOut)  shiftF = std::lerp(0.0f, float(cols + cfg_.headCols), 1.0f - t);
//
//        int shiftCols = int(std::floor(shiftF));
//
//        // 行ごとに head(先端) を計算
//        const float mid = (rows - 1) * 0.5f;
//
//        for (int y = 0; y < rows; ++y)
//        {
//            float ny = std::abs((float(y) - mid) / std::max<float>(1.0f, mid)); // 中央0～端1
//
//            // 先端（中央ほど前へ）
//            float headY = cfg_.headCols * std::powf(1.0f - ny, cfg_.headPow);
//
//            // ★後端（中央ほど深く削る）
//            float tailY = cfg_.tailCols * std::powf(1.0f - ny, cfg_.tailPow);
//
//            // 行yで塗る範囲（画面内だけ）
//            int xMin = shiftCols + int(std::round(tailY));                // ← ここで“＞”の切れ込み
//            int xMax = shiftCols + cfg_.bodyCols - 1 + int(std::round(headY));
//
//            int drawMin = std::max<int>(0, xMin);
//            int drawMax = std::min<int>(cols - 1, xMax);
//			if (drawMin <= drawMax)
//            {
//                for (int x = drawMin; x <= drawMax; ++x)
//                {
//                    int px = physX(x);
//                    int idx = y * cols + px;
//
//                    viewSprites_[idx] += dt_ / (cfg_.holdSec / 4.0f);
//                    viewSprites_[idx] = std::min<float>(viewSprites_[idx], 1.0f);
//                    const float colW = baseW_ / cfg_.cols;
//                    const float rowH = baseH_ / cfg_.rows;
//                    sprites_[idx].SetSize({ colW * viewSprites_[idx], rowH * viewSprites_[idx] });
//                    sprites_[idx].Update();
//                    sprites_[idx].Draw();
//                }
//            }
//
//            if (phase_ == FadePhase::FadeOut)
//            {
//                if (drawMin <= drawMax)
//                {
//                    for (int x = 0; x <= drawMin; ++x)
//                    {
//                        int px = physX(x);
//                        int idx = y * cols + px;
//
//                        if(viewSprites_[idx] >= 1.0f)
//                        {
//                            viewSprites_[idx] -= dt_ / (cfg_.holdSec / 4.0f);
//                            viewSprites_[idx] = std::max<float>(viewSprites_[idx], 0.0f);
//                        }
//                    }
//                }
//                for (int x = 0; x < cols; ++x)
//                {
//                    int px = physX(x);
//                    int idx = y * cols + px;
//
//                    if (viewSprites_[idx] > 0.0f)
//                    {
//                        if (viewSprites_[idx] < 1.0f)
//                        {
//                            viewSprites_[idx] -= dt_ / (cfg_.holdSec / 4.0f);
//                            viewSprites_[idx] = std::max<float>(viewSprites_[idx], 0.0f);
//                        }
//                        const float colW = baseW_ / cfg_.cols;
//                        const float rowH = baseH_ / cfg_.rows;
//                        sprites_[idx].SetSize({ colW * viewSprites_[idx], rowH * viewSprites_[idx] });
//                        sprites_[idx].Update();
//                        sprites_[idx].Draw();
//                    }
//                }
//            }
//
//        }
//    }
//
//
//
//
//    // cols/rows を変えたい場合は再Init
//    const BlockFadeConfig& GetConfig() const { return cfg_; }
//
//private:
//    static float saturate(float v) { return std::max<float>(0.0f, std::min<float>(1.0f, v)); }
//
//    BlockFadeConfig cfg_;
//    std::vector<Sprite> sprites_;   // タイル＝スプライト
//    std::vector<float> viewSprites_;
//    float baseW_ = 1280.0f, baseH_ = 720.0f;
//
//    FadePhase phase_ = FadePhase::Idle;
//    float timer_ = 0.0f;
//    float dt_ = 0;
//};
//
