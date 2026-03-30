#include "MyAnalyzerXAPO.h"
#include "mathFunc.h"
#include "Timer.h"
#include "PostEffectManager.h"
#include "VignetteEffect.h"
#include <imgui.h>
#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <numbers>


namespace TYEngine
{
    using namespace Utility;
    using namespace OffScreen;

	namespace AudioSystem
	{
        // 状態管理用の変数（クラスのメンバ変数など）
        float heartbeatPhase = 0.0f;

        void  MyAnalyzerXAPO::UpdateCriticalHealthEffects(
            float deltaTime,
            float healthPercent, // 0.0 (死亡) ～ 1.0 (満タン)
            TYEngine::AudioSystem::BiquadFilter& lpFilter,
            TYEngine::AudioSystem::BiquadFilter& lsFilter)
        {
            // --- 1. 閾値チェック (4.5割以下のみ適用) ---
            const float threshold = 0.45f;

            auto* pem = PostEffectManager::GetInstance();
            
            if (healthPercent > threshold)
            {
                // 3割以上の時はフィルターをリセットして効果を無効化
                // 以前の履歴が残っているとプチノイズが乗る可能性があるため Reset が安全
                lpFilter.Reset();
                lsFilter.Reset();

                // フィルター係数も「何もしない（全通）」状態にしておく
                lpFilter.SetCoefficients(TYEngine::AudioSystem::FilterType::LowPassFilter, 48000.0f, 20000.0f, 0.707f);
                lsFilter.SetCoefficients(TYEngine::AudioSystem::FilterType::LowShelf, 48000.0f, 10.0f, 0.707f, 0.0f);

                pem->SetEffectEnabled("HealthVignette", false);
                pem->GetEffect<VignetteEffect>("HealthVignette")->SetIntensity(0.0f);
                pem->GetEffect<VignetteEffect>("HealthVignette")->SetPower(0.0f);
                pem->GetEffect<VignetteEffect>("HealthVignette")->SetColor({ 0.0f,0.0f,0.0f });

                heartbeatPhase = 0.0f; // フェーズも初期化しておく
                return;
            }
            pem->SetEffectEnabled("HealthVignette", true);

            // --- 2. 演出強度の正規化 (0.0 ～ 1.0) ---
            float intensityMod = std::clamp(1.0f - (healthPercent / threshold), 0.0f, 1.0f);

            // --- 3. 心拍リズム ---
            // ゆっくりとした鼓動 (0.1Hz ～ 0.2Hz)
            float heartRate = std::lerp(0.1f, 0.2f, healthPercent / threshold);
            heartbeatPhase += heartRate * deltaTime;
            if (heartbeatPhase > 1.0f) heartbeatPhase -= 1.0f;

            // 鋭い拍動
            float pulse = std::powf(std::sinf(heartbeatPhase * std::numbers::pi_v<float> *2.0f), 8.0f);

            // --- 4. オーディオ適用 ---
            // LowPass: 拍動に合わせて 200Hz までこもらせる
            float lpCutoff = std::lerp(7000.0f, 300.0f, intensityMod);
            lpFilter.SetCoefficients(TYEngine::AudioSystem::FilterType::LowPassFilter, 48000.0f, lpCutoff, 0.707f);

            // LowShelf: 低音を 12dB ブースト
            float boostDb = std::lerp(0.0f, 12.0f, pulse * intensityMod);
            lsFilter.SetCoefficients(TYEngine::AudioSystem::FilterType::LowShelf, 48000.0f, 100.0f, 0.707f, boostDb);

            // --- 5. ヴィネット適用 ---
            // Intensity: 小さいほど視界が狭まる。
            // 通常時 30.0 -> 拍動の最大瞬間で 5.0 程度まで下げることで、周囲を暗く包み込む
            float currentVignetteIntensity = std::lerp(45.0f, 33.0f, pulse * intensityMod);
            pem->GetEffect<VignetteEffect>("HealthVignette")->SetIntensity(currentVignetteIntensity);

            // Power: 大きいほど減衰が急激（中心が狭く）なる。
            // 通常時 0.8 -> 最大瞬間 2.5 程度まで上げることで、トンネル視界を表現
            float currentPower = std::lerp(0.8f, 2.5f, pulse * intensityMod);
            pem->GetEffect<VignetteEffect>("HealthVignette")->SetPower(currentPower);

            // Color: HPが減るほど、周辺の闇に赤みが混じる
            Utility::Vector3 panicColor = { intensityMod * 0.5f, 0.0f, 0.0f }; // 暗い赤
            pem->GetEffect<VignetteEffect>("HealthVignette")->SetColor(panicColor);
        }

        void MyAnalyzerXAPO::TestBeat()
        {
            static float health = 1.0f;

            ImGui::DragFloat("health per", &health, 0.01f);

            for (int ch = 0; ch < static_cast<int>(channels_); ch++)
            {
                UpdateCriticalHealthEffects(
                    Timer::GetInstance()->GetDeltaTime(),
                    health,
                    eqFilters_[EQBand::LPF][ch],
                    eqFilters_[EQBand::Low][ch]);
            }
        }

        void MyAnalyzerXAPO::PerfBeat(float perf)
        {
            for (int ch = 0; ch < static_cast<int>(channels_); ch++)
            {
                UpdateCriticalHealthEffects(
                    Timer::GetInstance()->GetDeltaTime(),
                    perf,
                    eqFilters_[EQBand::LPF][ch],
                    eqFilters_[EQBand::Low][ch]);
            }
        }

	}
}
