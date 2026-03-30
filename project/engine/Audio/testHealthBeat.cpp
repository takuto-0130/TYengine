#include "MyAnalyzerXAPO.h"
#include "mathFunc.h"
//#include "../OffScreen/PostEffect/VignetteEffect.h"
#include <imgui.h>
#include <cmath>
#include <cstring>
#include <numbers>

//namespace TYEngine
//{
//	namespace AudioSystem
//	{
//        // 状態管理用の変数（クラスのメンバ変数など）
//        float heartbeatPhase = 0.0f;
//
//        void UpdateCriticalHealthEffects(
//            float deltaTime,
//            float healthPercent, // 0.0 (死亡) ～ 1.0 (満タン)
//            TYEngine::AudioSystem::BiquadFilter& audioFilter/*,
//            TYEngine::OffScreen::VignetteEffect& vignette*/)
//        {
//            // --- 1. 基本パラメータの設定 ---
//            // 体力が減るほど心拍を速くする (1.0Hz ～ 2.5Hz)
//            float heartRate = Utility::Lerp(2.5f, 1.0f, healthPercent);
//            heartbeatPhase += heartRate * deltaTime;
//            if (heartbeatPhase > 1.0f) heartbeatPhase -= 1.0f;
//
//            // 心音特有の「ドックン」という2峰性の波形を作る
//            // sin^4 または sin^8 を使うと、鋭い拍動が作れます
//            float pulse = std::powf(std::sinf(heartbeatPhase * std::numbers::pi_v<float>), 8.0f);
//
//            // 体力が低いほど全体的なエフェクトの「ベース強度」を上げる
//            float intensityModifier = 1.0f - healthPercent;
//
//            // --- 2. オーディオ（BiquadFilter）への適用 ---
//            // 拍動に合わせて 300Hz ～ 2000Hz の間を動かす
//            float cutoff = std::lerp(2000.0f, 300.0f, pulse * intensityModifier);
//            audioFilter.SetCoefficients(
//                TYEngine::AudioSystem::FilterType::LowPassFilter,
//                48000.0f, cutoff, 1.5f);
//
//            // --- 3. ビジュアル（VignetteEffect）への適用 ---
//            // 拍動に合わせて周辺減光を強くする
//            // 通常時(10.0) ～ 最大(30.0) などの範囲で調整
//            float baseVignette = 10.0f;
//            float maxVignette = 35.0f;
//            float currentVignette = std::lerp(baseVignette, maxVignette, pulse * intensityModifier);
//            //vignette.SetIntensity(currentVignette);
//
//            //// 瀕死時は周辺の色を「赤」に近づける演出
//            //Utility::Vector3 panicColor = { 0.5f, 0.0f, 0.0f }; // 暗い赤
//            //vignette.SetColor(panicColor);
//        }
//
//	}
//}
