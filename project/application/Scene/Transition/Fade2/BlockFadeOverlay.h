// BlockFadeOverlay.h
#pragma once
#include "Sprite.h"
#include "DirectXBasis.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

enum class FadePhase { Idle, FadeIn, Hold, FadeOut };

enum class FlowMode { Horizontal, DiagonalSimple, DiagonalArrow };

struct BlockFadeConfig
{
    int   cols = 32;
    int   rows = 18;
    float inSec = 1.5f;
    float holdSec = 1.5f;
    float outSec = 1.5f;
    float easePow = 1.0f;        // 1=linear, 2=quad...
    bool  leftToRight = true;   // falseで右→左
    float blackThreshold = 0.5f;// 平均輝度 ≤ しきい値 → 黒タイル扱い

    // 斜め用：1行あたり何カラムずらすか（列単位）
    float diagSlopeCols = 0.25f;   // 例：下へ1行進むごとに+0.25列ぶん先行

    // 矢印用：中央先行の振幅（列単位）とカーブ
    float arrowAmpCols = 4.0f;    // 中央がこれだけ先行（列）
    float arrowPow = 1.5f;    // 1で直線 / 大きいほど中央がさらに先行
    FlowMode flowMode = FlowMode::Horizontal;
};

// スプライト配列フェード
class BlockFadeOverlay
{
public:
    void Init(const std::string& white1x1TexPath, const BlockFadeConfig& cfg)
    {
        cfg_ = cfg;
        baseW_ = 1280.0f; baseH_ = 720.0f;

        // タイル数ぶん Sprite を確保（各タイル=1スプライト）
        sprites_.clear();
        sprites_.resize(cfg_.cols * cfg_.rows);

        const float colW = baseW_ / cfg_.cols;
        const float rowH = baseH_ / cfg_.rows;

        for (int y = 0; y < cfg_.rows; ++y)
        {
            for (int x = 0; x < cfg_.cols; ++x)
            {
                auto& sp = sprites_[y * cfg_.cols + x];
                sp.Initialize(white1x1TexPath); // 同じ白1x1を使う
                sp.SetAnchorPoint({ 0,0 });
                sp.SetColor({ 0,0,0,1 });        // 黒で塗る

                // 1280x720基準で固定配置（スケーリングしない）
                float px = x * colW;
                float py = y * rowH;
                sp.SetPosition({ px, py });
                sp.SetSize({ colW, rowH });
                sp.Update(); // 初期行列だけ作っておく（毎フレーム変更しない）
            }
        }

        // マスクは後で LoadMaskImage() で与える
        tileMask_.assign(cfg_.cols * cfg_.rows, true); // 既定は全タイル有効
    }

    // マスク画像（自作画像）を読み込み → タイルのON/OFFへ縮約
    bool LoadMaskImage(const std::wstring& filePathW)
    {
        using namespace DirectX;
        ScratchImage img;
        HRESULT hr;
        if (filePathW.size() >= 4 && _wcsicmp(filePathW.c_str() + filePathW.size() - 4, L".dds") == 0)
            hr = LoadFromDDSFile(filePathW.c_str(), DDS_FLAGS_NONE, nullptr, img);
        else
            hr = LoadFromWICFile(filePathW.c_str(), WIC_FLAGS_FORCE_SRGB, nullptr, img);
        if (FAILED(hr)) return false;

        const Image* im = img.GetImage(0, 0, 0);
        int maskW = int(im->width);
        int maskH = int(im->height);

        tileMask_.assign(cfg_.cols * cfg_.rows, false);

        for (int ty = 0; ty < cfg_.rows; ++ty)
        {
            for (int tx = 0; tx < cfg_.cols; ++tx)
            {
                int x0 = int(tx * (float)maskW / cfg_.cols);
                int x1 = int((tx + 1) * (float)maskW / cfg_.cols);
                int y0 = int(ty * (float)maskH / cfg_.rows);
                int y1 = int((ty + 1) * (float)maskH / cfg_.rows);
                x0 = std::max<int>(0, x0); y0 = std::max<int>(0, y0);
                x1 = std::min<int>(maskW, x1); y1 = std::min<int>(maskH, y1);

                double sum = 0.0; int cnt = 0;
                for (int y = y0; y < y1; ++y)
                {
                    const uint8_t* row = im->pixels + y * im->rowPitch;
                    for (int x = x0; x < x1; ++x)
                    {
                        const uint8_t* px = row + x * 4; // RGBA8想定
                        double r = px[0] / 255.0, g = px[1] / 255.0, b = px[2] / 255.0;
                        sum += 0.2126 * r + 0.7152 * g + 0.0722 * b; // 輝度
                        ++cnt;
                    }
                }
                double avg = (cnt > 0) ? (sum / cnt) : 1.0;
                bool isBlack = (avg <= cfg_.blackThreshold);
                tileMask_[ty * cfg_.cols + tx] = isBlack;
            }
        }
        return true;
    }

    // フェーズ制御
    void StartFadeIn() { phase_ = FadePhase::FadeIn;  timer_ = 0; }
    void StartHold() { phase_ = FadePhase::Hold;    timer_ = 0; }
    void StartFadeOut() { phase_ = FadePhase::FadeOut; timer_ = 0; }
    bool IsBusy() const { return phase_ != FadePhase::Idle; }
    FadePhase Phase() const { return phase_; }

    void Update(float dt)
    {
        if (phase_ == FadePhase::Idle) return;
        timer_ += dt;
        switch (phase_)
        {
        case FadePhase::FadeIn:  if (timer_ >= cfg_.inSec) { timer_ = 0; phase_ = FadePhase::Hold; } break;
        case FadePhase::Hold:    if (timer_ >= cfg_.holdSec) { timer_ = 0; phase_ = FadePhase::FadeOut; } break;
        case FadePhase::FadeOut: if (timer_ >= cfg_.outSec) { timer_ = 0; phase_ = FadePhase::Idle; } break;
        default: break;
        }
    }

    // フレームの一番最後（UI含む全描画の後）に呼ぶ
    void Draw()
    {
        if (phase_ == FadePhase::Idle) return;

        const int cols = std::max<int>(1, cfg_.cols);
        const int rows = std::max<int>(1, cfg_.rows);

        auto sat = [](float v) { return std::max<float>(0.0f, std::min(1.0f, v)); };

        // 進行度 t
        float t = 0.0f;
        if (phase_ == FadePhase::FadeIn)  t = sat(timer_ / cfg_.inSec);
        else if (phase_ == FadePhase::Hold)    t = 1.0f;
        else if (phase_ == FadePhase::FadeOut) t = 1.0f - sat(timer_ / cfg_.outSec);
        t = std::powf(t, cfg_.easePow);

        // 横シフトの基準（列単位）
        auto baseShiftCols = [&](FadePhase ph)->float
            {
                switch (ph)
                {
                case FadePhase::FadeIn:  return -(1.0f - t) * cols; // -cols → 0
                case FadePhase::Hold:    return 0.0f;
                case FadePhase::FadeOut: return  (1.0f - t) * cols; // 0 → +cols
                default: return 0.0f;
                }
            }(phase_);

        // 画面側のXインデックス（右→左に流したいなら反転）
        auto physX = [&](int x)->int { return cfg_.leftToRight ? x : (cols - 1 - x); };

        // 行ごとに「横シフト」を決める
        auto rowShiftCols = [&](int y)->int
            {
                float s = baseShiftCols; // 基準（全行共通）

                if (cfg_.flowMode == FlowMode::DiagonalSimple)
                {
                    // 下へ行くほど先行（斜めの前縁）
                    // 例：diagSlopeCols=0.25 → 4行で1列ぶん先行
                    s += cfg_.diagSlopeCols * float(y);
                }
                else if (cfg_.flowMode == FlowMode::DiagonalArrow)
                {
                    // 中央が最も先行、上下は遅れる（矢印/V字）
                    float mid = (rows - 1) * 0.5f;
                    float ny = std::abs((float(y) - mid) / std::max<float>(1.0f, mid)); // 0(中央)～1(端)
                    float prof = std::powf(1.0f - ny, cfg_.arrowPow);              // 中央1, 端0
                    s += cfg_.arrowAmpCols * prof;
                }
                // Horizontal はそのまま

                // 端数は最寄りの列に
                return int(std::round(s));
            };

        // 描画（1280×720基準で配置済みスプライトを使う）
        for (int y = 0; y < rows; ++y)
        {
            int shiftCols = rowShiftCols(y);

            for (int x = 0; x < cols; ++x)
            {
                int px = physX(x);                    // 画面側の列
                int sx = x - shiftCols;               // 元マスクの参照列（横に流す）

                if (sx < 0 || sx >= cols) continue;   // 画面外＝白扱い（描かない）

                int srcIdx = y * cols + sx;          // マスク参照
                if (!tileMask_.empty() && !tileMask_[srcIdx]) continue; // 白は描かない

                int drawIdx = y * cols + px;          // 既設スプライトを描画
                sprites_[drawIdx].Draw();
            }
        }
    }

    // cols/rows を変えたい場合は再Init
    const BlockFadeConfig& GetConfig() const { return cfg_; }

private:
    static float saturate(float v) { return std::max<float>(0.0f, std::min<float>(1.0f, v)); }

    BlockFadeConfig cfg_;
    std::vector<Sprite> sprites_;   // タイル＝スプライト
    std::vector<bool>   tileMask_;  // 画像→タイルON/OFF
    float baseW_ = 1280.0f, baseH_ = 720.0f;

    FadePhase phase_ = FadePhase::Idle;
    float timer_ = 0.0f;
};

