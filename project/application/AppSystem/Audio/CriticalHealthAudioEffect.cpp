#include "CriticalHealthAudioEffect.h"
#include "MyAnalyzerXAPO.h"
#include "PostEffectManager.h"
#include "VignetteEffect.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace TYEngine
{
    using namespace Utility;
    using namespace OffScreen;

    CriticalHealthAudioEffect::CriticalHealthAudioEffect()
        : heartbeatPhase_(0.0f)
    {
    }

    void CriticalHealthAudioEffect::Update(float deltaTime, float healthPercent, Microsoft::WRL::ComPtr<AudioSystem::MyAnalyzerXAPO> analyzer)
    {
        if (!analyzer) return;

        // --- 1. 閾値チェック (4.5割以下のみ適用) ---
        const float threshold = 0.45f;
        auto* pem = PostEffectManager::GetInstance();

        if (healthPercent > threshold)
        {
            // 閾値以上の時はフィルターを通常設定に戻す
            analyzer->SetFiltersHz(20000.0f, 10.0f, 2000.0f);
            analyzer->SetEQGain(0.0f, 0.0f, 0.0f);

            if (pem)
            {
                pem->SetEffectEnabled("HealthVignette", false);
                auto* vignette = pem->GetEffect<VignetteEffect>("HealthVignette");
                if (vignette)
                {
                    vignette->SetIntensity(0.0f);
                    vignette->SetPower(0.0f);
                    vignette->SetColor({ 0.0f, 0.0f, 0.0f });
                }
            }

            heartbeatPhase_ = 0.0f; // フェーズも初期化しておく
            return;
        }

        if (pem)
        {
            pem->SetEffectEnabled("HealthVignette", true);
        }

        // --- 2. 演出強度の正規化 (0.0 ～ 1.0) ---
        float intensityMod = std::clamp(1.0f - (healthPercent / threshold), 0.0f, 1.0f);

        // --- 3. 心拍リズム ---
        // ゆっくりとした鼓動 (0.1Hz ～ 0.2Hz)
        float heartRate = std::lerp(0.1f, 0.2f, healthPercent / threshold);
        heartbeatPhase_ += heartRate * deltaTime;
        if (heartbeatPhase_ > 1.0f) heartbeatPhase_ -= 1.0f;

        // 鋭い拍動
        float pulse = std::powf(std::sinf(heartbeatPhase_ * std::numbers::pi_v<float> * 2.0f), 8.0f);

        // --- 4. オーディオ適用 ---
        // LowPass: 拍動に合わせて 200Hz までこもらせる (元のコードでは400Hz)
        float lpCutoff = std::lerp(7000.0f, 400.0f, intensityMod);

        // LowShelf: 低音を 12dB ブースト
        float boostDb = std::lerp(0.0f, 12.0f, pulse * intensityMod);

        analyzer->SetFiltersHz(lpCutoff, 10.0f, 2000.0f);
        analyzer->SetEQGain(boostDb, 0.0f, 0.0f);

        // --- 5. ヴィネット適用 ---
        if (pem)
        {
            auto* vignette = pem->GetEffect<VignetteEffect>("HealthVignette");
            if (vignette)
            {
                // Intensity: 小さいほど視界が狭まる。
                float currentVignetteIntensity = std::lerp(45.0f, 33.0f, pulse * intensityMod);
                vignette->SetIntensity(currentVignetteIntensity);

                // Power: 大きいほど減衰が急激（中心が狭く）なる。
                float currentPower = std::lerp(0.8f, 2.5f, pulse * intensityMod);
                vignette->SetPower(currentPower);

                // Color: HPが減るほど、周辺 of 闇に赤みが混じる
                Utility::Vector3 panicColor = { intensityMod * 0.5f, 0.0f, 0.0f }; // 暗い赤
                vignette->SetColor(panicColor);
            }
        }
    }
}
