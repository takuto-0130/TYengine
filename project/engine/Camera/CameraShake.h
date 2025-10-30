#pragma once
#include "mathFunc.h"
#include <random>

class CameraShake
{
public:
    // パラメータ
    struct ShakeParams
    {
        float duration = 0.5f;     // シェイク継続時間
        float amplitude = 0.5f;    // 揺れの強さ
        float frequency = 20.0f;   // ノイズ周波数（1秒あたりの揺れ更新回数）
        bool decay = true;         // 時間経過で減衰するか
    };

public:
    CameraShake() = default;

    // シェイク開始
    void Start(const ShakeParams& params)
    {
        params_ = params;
        time_ = 0.0f;
        active_ = true;
    }

    // 更新
    void Update(float deltaTime)
    {
        if (!active_) return;

        time_ += deltaTime;
        if (time_ >= params_.duration)
        {
            active_ = false;
            offset_ = { 0, 0, 0 };
            return;
        }

        float progress = time_ / params_.duration;

        float amplitude = params_.decay ? params_.amplitude * (1.0f - progress) : params_.amplitude;
        float t = progress * params_.frequency;

        offset_.x = amplitude * sinf(t * 13.0f + RandomOffset(0));
        offset_.y = amplitude * sinf(t * 17.0f + RandomOffset(1));
        offset_.z = amplitude * sinf(t * 11.0f + RandomOffset(2));
    }

    // 現在のオフセットを取得
    const Vector3& GetOffset() const { return offset_; }

    // アクティブかどうか
    bool IsActive() const { return active_; }

private:
    float RandomOffset(int seed)
    {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 3.14159f * 2.0f);
        rng.seed(seed * 131);
        return dist(rng);
    }

private:
    ShakeParams params_;
    float time_ = 0.0f;
    bool active_ = false;
    Vector3 offset_ = {};
};
