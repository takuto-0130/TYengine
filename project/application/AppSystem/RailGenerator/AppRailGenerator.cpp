#include "Audio.h"
#include "OfflineAudioAnalyzer.h"
#include "../../Object/Rail/RailManager.h"
#include "AppRailGenerator.h"
#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"
#include <imgui.h>

static std::string g_detectedStyle = "None";
static float g_avgLow = 0.0f;
static float g_avgMid = 0.0f;
static float g_avgHigh = 0.0f;
static float g_avgAll = 0.0f;
static float g_bpm = 0.0f;

using namespace TYEngine::AudioSystem;

void GenerateStageFromAudio(const std::string& soundFilename, RailManager* railManager)
{
    TYEngine::Utility::JsonManager jm;
    std::string err;
    jm.Load("RailGenerator.json", true, &err);

    // デフォルト値
    if (!jm.Root().contains("advanceZ")) jm.Set("advanceZ", 20.0f);
    if (!jm.Root().contains("smoothFactor")) jm.Set("smoothFactor", 1.0f);
    if (!jm.Root().contains("scaleX")) jm.Set("scaleX", 400.0f);
    if (!jm.Root().contains("scaleY")) jm.Set("scaleY", 150.0f);
    if (!jm.Root().contains("scaleLow")) jm.Set("scaleLow", 200.0f);
    if (!jm.Root().contains("scaleHigh")) jm.Set("scaleHigh", 100.0f);
    if (!jm.Root().contains("wavePeriod")) jm.Set("wavePeriod", 8.0f);
    if (!jm.Root().contains("lowThreshold")) jm.Set("lowThreshold", 0.08f);
    if (!jm.Root().contains("clampX")) jm.Set("clampX", 50.0f);
    if (!jm.Root().contains("span")) jm.Set("span", 4);

    // ジャンル別デフォルト値
    if (!jm.Root().contains("genres")) jm.Set("genres", nlohmann::json::object());
    if (!jm.Root()["genres"].contains("Ambient")) {
        jm.Set("genres.Ambient.thresholdAvgAll", 0.04f);
        jm.Set("genres.Ambient.modScaleX", 0.2f);
        jm.Set("genres.Ambient.modScaleY", 0.2f);
        jm.Set("genres.Ambient.modScaleLow", 0.1f);
        jm.Set("genres.Ambient.modScaleHigh", 0.1f);
        jm.Set("genres.Ambient.modSmoothFactor", 0.1f);
        jm.Set("genres.Ambient.modSpan", 8);
    }
    if (!jm.Root()["genres"].contains("EDM")) {
        jm.Set("genres.EDM.thresholdLowRatio", 0.42f);
        jm.Set("genres.EDM.thresholdBPM", 120.0f);
        jm.Set("genres.EDM.modScaleX", 1.3f);
        jm.Set("genres.EDM.modScaleY", 1.0f);
        jm.Set("genres.EDM.modScaleLow", 2.5f);
        jm.Set("genres.EDM.modScaleHigh", 1.0f);
        jm.Set("genres.EDM.modWavePeriod", 4.0f);
        jm.Set("genres.EDM.modSmoothFactor", 1.0f);
        jm.Set("genres.EDM.modSpan", 2);
    }
    if (!jm.Root()["genres"].contains("Melodic")) {
        jm.Set("genres.Melodic.thresholdMidRatio", 0.45f);
        jm.Set("genres.Melodic.thresholdHighRatio", 0.35f);
        jm.Set("genres.Melodic.modScaleX", 2.0f);
        jm.Set("genres.Melodic.modScaleY", 1.0f);
        jm.Set("genres.Melodic.modScaleLow", 0.5f);
        jm.Set("genres.Melodic.modScaleHigh", 1.0f);
        jm.Set("genres.Melodic.modWavePeriod", 12.0f);
        jm.Set("genres.Melodic.modSmoothFactor", 1.0f);
        jm.Set("genres.Melodic.modSpan", 4);
    }
    if (!jm.Root()["genres"].contains("Metal")) {
        jm.Set("genres.Metal.thresholdBPM", 140.0f);
        jm.Set("genres.Metal.thresholdAvgAll", 0.08f);
        jm.Set("genres.Metal.modScaleX", 1.5f);
        jm.Set("genres.Metal.modScaleY", 1.0f);
        jm.Set("genres.Metal.modScaleLow", 1.6f);
        jm.Set("genres.Metal.modScaleHigh", 1.0f);
        jm.Set("genres.Metal.modWavePeriod", 6.0f);
        jm.Set("genres.Metal.modSmoothFactor", 0.8f);
        jm.Set("genres.Metal.modSpan", 2);
    }

    jm.Save();

    float advanceZ = jm.Get<float>("advanceZ", 20.0f);
    float smoothFactor = jm.Get<float>("smoothFactor", 1.0f);
    float scaleX = jm.Get<float>("scaleX", 400.0f);
    float scaleY = jm.Get<float>("scaleY", 150.0f);
    float scaleLow = jm.Get<float>("scaleLow", 200.0f);
    float scaleHigh = jm.Get<float>("scaleHigh", 100.0f);
    float wavePeriod = jm.Get<float>("wavePeriod", 8.0f);
    float lowThreshold = jm.Get<float>("lowThreshold", 0.08f);
    float clampX = jm.Get<float>("clampX", 50.0f);
    int span = jm.Get<int>("span", 4);

    // ジャンル別パラメータのロード
    float ambient_thresholdAvgAll = jm.Get<float>("genres.Ambient.thresholdAvgAll", 0.04f);
    float ambient_modScaleX = jm.Get<float>("genres.Ambient.modScaleX", 0.2f);
    float ambient_modScaleY = jm.Get<float>("genres.Ambient.modScaleY", 0.2f);
    float ambient_modScaleLow = jm.Get<float>("genres.Ambient.modScaleLow", 0.1f);
    float ambient_modScaleHigh = jm.Get<float>("genres.Ambient.modScaleHigh", 0.1f);
    float ambient_modSmoothFactor = jm.Get<float>("genres.Ambient.modSmoothFactor", 0.1f);
    int ambient_modSpan = jm.Get<int>("genres.Ambient.modSpan", span * 2);

    float edm_thresholdLowRatio = jm.Get<float>("genres.EDM.thresholdLowRatio", 0.42f);
    float edm_thresholdBPM = jm.Get<float>("genres.EDM.thresholdBPM", 120.0f);
    float edm_modScaleX = jm.Get<float>("genres.EDM.modScaleX", 1.3f);
    float edm_modScaleY = jm.Get<float>("genres.EDM.modScaleY", 1.0f);
    float edm_modScaleLow = jm.Get<float>("genres.EDM.modScaleLow", 2.5f);
    float edm_modScaleHigh = jm.Get<float>("genres.EDM.modScaleHigh", 1.0f);
    float edm_modWavePeriod = jm.Get<float>("genres.EDM.modWavePeriod", 4.0f);
    float edm_modSmoothFactor = jm.Get<float>("genres.EDM.modSmoothFactor", 1.0f);
    int edm_modSpan = jm.Get<int>("genres.EDM.modSpan", 2);

    float melodic_thresholdMidRatio = jm.Get<float>("genres.Melodic.thresholdMidRatio", 0.45f);
    float melodic_thresholdHighRatio = jm.Get<float>("genres.Melodic.thresholdHighRatio", 0.35f);
    float melodic_modScaleX = jm.Get<float>("genres.Melodic.modScaleX", 2.0f);
    float melodic_modScaleY = jm.Get<float>("genres.Melodic.modScaleY", 1.0f);
    float melodic_modScaleLow = jm.Get<float>("genres.Melodic.modScaleLow", 0.5f);
    float melodic_modScaleHigh = jm.Get<float>("genres.Melodic.modScaleHigh", 1.0f);
    float melodic_modWavePeriod = jm.Get<float>("genres.Melodic.modWavePeriod", 12.0f);
    float melodic_modSmoothFactor = jm.Get<float>("genres.Melodic.modSmoothFactor", 1.0f);
    int melodic_modSpan = jm.Get<int>("genres.Melodic.modSpan", 4);

    float metal_thresholdBPM = jm.Get<float>("genres.Metal.thresholdBPM", 140.0f);
    float metal_thresholdAvgAll = jm.Get<float>("genres.Metal.thresholdAvgAll", 0.08f);
    float metal_modScaleX = jm.Get<float>("genres.Metal.modScaleX", 1.5f);
    float metal_modScaleY = jm.Get<float>("genres.Metal.modScaleY", 1.0f);
    float metal_modScaleLow = jm.Get<float>("genres.Metal.modScaleLow", 1.6f);
    float metal_modScaleHigh = jm.Get<float>("genres.Metal.modScaleHigh", 1.0f);
    float metal_modWavePeriod = jm.Get<float>("genres.Metal.modWavePeriod", 6.0f);
    float metal_modSmoothFactor = jm.Get<float>("genres.Metal.modSmoothFactor", 0.8f);
    int metal_modSpan = jm.Get<int>("genres.Metal.modSpan", 2);

    // エンジンからWAVEの生データを取得する
    auto soundData = Audio::GetInstance()->GetSoundData(soundFilename);

    // エンジンに解析を依頼し、純粋なオーディオ特徴量の配列をもらう
    AudioAnalysisResult analysisData = OfflineAudioAnalyzer::Analyze(
        soundData.buffer.data(), static_cast<UINT32>(soundData.buffer.size()), soundData.wfex
    );

    // --- 音楽のジャンル/スタイル自動判定とパラメータ調整 ---
    float avgLow = analysisData.avgLow;
    float avgMid = analysisData.avgMid;
    float avgHigh = analysisData.avgHigh;
    float avgAll = analysisData.avgAll;
    float totalRatio = avgLow + avgMid + avgHigh;

    float lowRatio = totalRatio > 0.0f ? (avgLow / totalRatio) : 0.0f;
    float midRatio = totalRatio > 0.0f ? (avgMid / totalRatio) : 0.0f;
    float highRatio = totalRatio > 0.0f ? (avgHigh / totalRatio) : 0.0f;

    // 分類に応じたパラメータ補正係数
    float modScaleX = 1.0f;
    float modScaleY = 1.0f;
    float modScaleLow = 1.0f;
    float modScaleHigh = 1.0f;
    int modSpan = span;
    float modWavePeriod = wavePeriod;
    float modSmoothFactor = smoothFactor;

    std::string detectedStyle = "Standard Pop/Rock";

    // 1. Ambient / Chillout (極めて静かな曲、または起伏の少ない曲)
    if (avgAll < ambient_thresholdAvgAll)
    {
        detectedStyle = "Ambient / Chillout";
        modScaleX = ambient_modScaleX;
        modScaleY = ambient_modScaleY;
        modScaleLow = ambient_modScaleLow;
        modScaleHigh = ambient_modScaleHigh;
        modSmoothFactor = ambient_modSmoothFactor;
        modSpan = ambient_modSpan;
    }
    // 2. EDM / Dance (低音重視 & ビートが速い)
    else if (lowRatio > edm_thresholdLowRatio && analysisData.bpm >= edm_thresholdBPM)
    {
        detectedStyle = "EDM / Dance";
        modScaleLow = edm_modScaleLow;
        modScaleX = edm_modScaleX;
        modScaleY = edm_modScaleY;
        modScaleHigh = edm_modScaleHigh;
        modWavePeriod = edm_modWavePeriod;
        modSmoothFactor = edm_modSmoothFactor;
        modSpan = edm_modSpan;
    }
    // 3. Melodic / Classical / Ballad (中高音重視)
    else if (midRatio > melodic_thresholdMidRatio || highRatio > melodic_thresholdHighRatio)
    {
        detectedStyle = "Melodic / Classical";
        modScaleX = melodic_modScaleX;
        modScaleY = melodic_modScaleY;
        modScaleLow = melodic_modScaleLow;
        modScaleHigh = melodic_modScaleHigh;
        modWavePeriod = melodic_modWavePeriod;
        modSmoothFactor = melodic_modSmoothFactor;
        modSpan = melodic_modSpan;
    }
    // 4. Hard Rock / Metal / Hardcore (高速 & 音圧が高い)
    else if (analysisData.bpm >= metal_thresholdBPM && avgAll > metal_thresholdAvgAll)
    {
        detectedStyle = "Hard Rock / Metal";
        modScaleX = metal_modScaleX;
        modScaleY = metal_modScaleY;
        modScaleLow = metal_modScaleLow;
        modScaleHigh = metal_modScaleHigh;
        modWavePeriod = metal_modWavePeriod;
        modSmoothFactor = metal_modSmoothFactor;
        modSpan = metal_modSpan;
    }

    // パラメータ適用
    scaleX *= modScaleX;
    scaleY *= modScaleY;
    scaleLow *= modScaleLow;
    scaleHigh *= modScaleHigh;
    span = modSpan;
    wavePeriod = modWavePeriod;
    smoothFactor = modSmoothFactor;

    // 判定された音楽スタイルや統計値をキャッシュ
    g_detectedStyle = detectedStyle;
    g_bpm = analysisData.bpm;
    g_avgLow = avgLow;
    g_avgMid = avgMid;
    g_avgHigh = avgHigh;
    g_avgAll = avgAll;

    // ゲーム固有のロジックでレール座標（制御点）に変換する
    std::vector<TYEngine::Utility::Vector3> newControlPoints;
    std::vector<bool> newTriggerFlags;

    float currentZ = 0.0f;
    float ADVANCE_Z = advanceZ; // 1ビートで進む距離

    newControlPoints.push_back({ 0.0f, 0.0f, 0.0f });
    newTriggerFlags.push_back(false);

    int index = 0;

    // 前回の平滑化された座標を保持する変数
    // スタート地点に合わせて 0.0f で初期化しておく
    float lastSmoothedX = 0.0f;
    float lastSmoothedY = 0.0f;

    // 平滑化の度合いを決める係数 (0.0f ～ 1.0f)
    // 1.0f だと平滑化なし（元のまま）。
    float SMOOTH_FACTOR = smoothFactor;

    for (const auto& beat : analysisData.beats)
    {
        if (index % span == 0)
        {
            currentZ += ADVANCE_Z;

            int stepIndex = index / span;

            // --- 左右の蛇行 (X座標) ---
            // S字状に交互にうねらせるためのサイン波
            float wavePhase = (static_cast<float>(stepIndex) * 2.0f * 3.14159265f) / wavePeriod;
            
            // 中音域（メロディ等）の強さに応じたサイン波蛇行
            float targetX = std::sin(wavePhase) * beat.rmsMid * scaleX;

            // 高音域（ハイハット等）の強さに応じた細かなうねりをブレンド
            float fastWave = std::cos(wavePhase * 3.0f) * beat.rmsHigh * scaleHigh;
            targetX += fastWave;

            // --- 高低差 (Y座標) ---
            // 全体音量に比例した基本の高さ
            float targetY = beat.rmsAll * scaleY;

            // 低音域（キック、ベース等）がしきい値を超えた場合、道が隆起する
            if (beat.rmsLow > lowThreshold)
            {
                targetY += (beat.rmsLow - lowThreshold) * scaleLow;
            }

            // 制限(クランプ)
            if (targetX > clampX) targetX = clampX;
            if (targetX < -clampX) targetX = -clampX;

            // 線形補間をかける
            // 前回の値 × (1 - 係数) ＋ 目標値 × (係数)
            lastSmoothedX = lastSmoothedX * (1.0f - SMOOTH_FACTOR) + targetX * SMOOTH_FACTOR;
            lastSmoothedY = lastSmoothedY * (1.0f - SMOOTH_FACTOR) + targetY * SMOOTH_FACTOR;

            // 平滑化された値(lastSmoothed)を配列に積む
            newControlPoints.push_back({ lastSmoothedX, lastSmoothedY, currentZ });
            // newTriggerFlags.push_back(true); // ビート位置に敵配置トリガーを置く等
        }
        index++;
    }

    // 生成したデータを RailManager に直接セットして再構築
    railManager->SetDynamicData(newControlPoints, newTriggerFlags);
    railManager->GenerateForest();
}

void DrawRailGeneratorDebugUI(const std::string& currentSoundFilename, RailManager* railManager)
{
#ifdef _DEBUG
    ImGui::Begin("Rail Generator Debug");

    ImGui::Text("Current Song: %s", currentSoundFilename.c_str());
    ImGui::Text("Detected Style: %s", g_detectedStyle.c_str());
    ImGui::Text("BPM: %.1f", g_bpm);
    ImGui::Separator();

    ImGui::Text("Average Amplitude Stats:");
    ImGui::Text("  All:  %.4f", g_avgAll);
    ImGui::Text("  Low:  %.4f", g_avgLow);
    ImGui::Text("  Mid:  %.4f", g_avgMid);
    ImGui::Text("  High: %.4f", g_avgHigh);
    ImGui::Separator();

    TYEngine::Utility::JsonManager jm;
    std::string err;
    if (jm.Load("RailGenerator.json", false, &err))
    {
        bool changed = false;

        // --- 基準値ロード ---
        float advanceZ = jm.Get<float>("advanceZ", 20.0f);
        float smoothFactor = jm.Get<float>("smoothFactor", 1.0f);
        float scaleX = jm.Get<float>("scaleX", 400.0f);
        float scaleY = jm.Get<float>("scaleY", 150.0f);
        float scaleLow = jm.Get<float>("scaleLow", 200.0f);
        float scaleHigh = jm.Get<float>("scaleHigh", 100.0f);
        float wavePeriod = jm.Get<float>("wavePeriod", 8.0f);
        float lowThreshold = jm.Get<float>("lowThreshold", 0.08f);
        float clampX = jm.Get<float>("clampX", 50.0f);
        int span = jm.Get<int>("span", 4);

        // --- ジャンル別値ロード ---
        // Ambient
        float ambient_thresholdAvgAll = jm.Get<float>("genres.Ambient.thresholdAvgAll", 0.04f);
        float ambient_modScaleX = jm.Get<float>("genres.Ambient.modScaleX", 0.2f);
        float ambient_modScaleY = jm.Get<float>("genres.Ambient.modScaleY", 0.2f);
        float ambient_modScaleLow = jm.Get<float>("genres.Ambient.modScaleLow", 0.1f);
        float ambient_modScaleHigh = jm.Get<float>("genres.Ambient.modScaleHigh", 0.1f);
        float ambient_modSmoothFactor = jm.Get<float>("genres.Ambient.modSmoothFactor", 0.1f);
        int ambient_modSpan = jm.Get<int>("genres.Ambient.modSpan", span * 2);

        // EDM
        float edm_thresholdLowRatio = jm.Get<float>("genres.EDM.thresholdLowRatio", 0.42f);
        float edm_thresholdBPM = jm.Get<float>("genres.EDM.thresholdBPM", 120.0f);
        float edm_modScaleX = jm.Get<float>("genres.EDM.modScaleX", 1.3f);
        float edm_modScaleY = jm.Get<float>("genres.EDM.modScaleY", 1.0f);
        float edm_modScaleLow = jm.Get<float>("genres.EDM.modScaleLow", 2.5f);
        float edm_modScaleHigh = jm.Get<float>("genres.EDM.modScaleHigh", 1.0f);
        float edm_modWavePeriod = jm.Get<float>("genres.EDM.modWavePeriod", 4.0f);
        float edm_modSmoothFactor = jm.Get<float>("genres.EDM.modSmoothFactor", 1.0f);
        int edm_modSpan = jm.Get<int>("genres.EDM.modSpan", 2);

        // Melodic
        float melodic_thresholdMidRatio = jm.Get<float>("genres.Melodic.thresholdMidRatio", 0.45f);
        float melodic_thresholdHighRatio = jm.Get<float>("genres.Melodic.thresholdHighRatio", 0.35f);
        float melodic_modScaleX = jm.Get<float>("genres.Melodic.modScaleX", 2.0f);
        float melodic_modScaleY = jm.Get<float>("genres.Melodic.modScaleY", 1.0f);
        float melodic_modScaleLow = jm.Get<float>("genres.Melodic.modScaleLow", 0.5f);
        float melodic_modScaleHigh = jm.Get<float>("genres.Melodic.modScaleHigh", 1.0f);
        float melodic_modWavePeriod = jm.Get<float>("genres.Melodic.modWavePeriod", 12.0f);
        float melodic_modSmoothFactor = jm.Get<float>("genres.Melodic.modSmoothFactor", 1.0f);
        int melodic_modSpan = jm.Get<int>("genres.Melodic.modSpan", 4);

        // Metal
        float metal_thresholdBPM = jm.Get<float>("genres.Metal.thresholdBPM", 140.0f);
        float metal_thresholdAvgAll = jm.Get<float>("genres.Metal.thresholdAvgAll", 0.08f);
        float metal_modScaleX = jm.Get<float>("genres.Metal.modScaleX", 1.5f);
        float metal_modScaleY = jm.Get<float>("genres.Metal.modScaleY", 1.0f);
        float metal_modScaleLow = jm.Get<float>("genres.Metal.modScaleLow", 1.6f);
        float metal_modScaleHigh = jm.Get<float>("genres.Metal.modScaleHigh", 1.0f);
        float metal_modWavePeriod = jm.Get<float>("genres.Metal.modWavePeriod", 6.0f);
        float metal_modSmoothFactor = jm.Get<float>("genres.Metal.modSmoothFactor", 0.8f);
        int metal_modSpan = jm.Get<int>("genres.Metal.modSpan", 2);

        // 基準値スライダー
        if (ImGui::CollapsingHeader("Base Generation Parameters", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::SliderFloat("Advance Z", &advanceZ, 5.0f, 50.0f)) { jm.Set("advanceZ", advanceZ); changed = true; }
            if (ImGui::SliderFloat("Smooth Factor", &smoothFactor, 0.01f, 1.0f)) { jm.Set("smoothFactor", smoothFactor); changed = true; }
            if (ImGui::SliderFloat("Scale X (Mid)", &scaleX, 50.0f, 1000.0f)) { jm.Set("scaleX", scaleX); changed = true; }
            if (ImGui::SliderFloat("Scale Y (All)", &scaleY, 10.0f, 500.0f)) { jm.Set("scaleY", scaleY); changed = true; }
            if (ImGui::SliderFloat("Scale Low (Bump)", &scaleLow, 10.0f, 1000.0f)) { jm.Set("scaleLow", scaleLow); changed = true; }
            if (ImGui::SliderFloat("Scale High (Chirp)", &scaleHigh, 10.0f, 500.0f)) { jm.Set("scaleHigh", scaleHigh); changed = true; }
            if (ImGui::SliderFloat("Wave Period", &wavePeriod, 2.0f, 32.0f)) { jm.Set("wavePeriod", wavePeriod); changed = true; }
            if (ImGui::SliderFloat("Low Threshold", &lowThreshold, 0.01f, 0.5f)) { jm.Set("lowThreshold", lowThreshold); changed = true; }
            if (ImGui::SliderFloat("Clamp X", &clampX, 10.0f, 200.0f)) { jm.Set("clampX", clampX); changed = true; }
            if (ImGui::SliderInt("Span", &span, 1, 16)) { jm.Set("span", span); changed = true; }
        }

        // ジャンル別パラメータの CollapsingHeader
        if (ImGui::CollapsingHeader("Genre Modifier Settings"))
        {
            if (ImGui::TreeNode("Ambient / Chillout"))
            {
                if (ImGui::SliderFloat("Threshold AvgAll", &ambient_thresholdAvgAll, 0.005f, 0.2f)) { jm.Set("genres.Ambient.thresholdAvgAll", ambient_thresholdAvgAll); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleX", &ambient_modScaleX, 0.0f, 3.0f)) { jm.Set("genres.Ambient.modScaleX", ambient_modScaleX); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleY", &ambient_modScaleY, 0.0f, 3.0f)) { jm.Set("genres.Ambient.modScaleY", ambient_modScaleY); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleLow", &ambient_modScaleLow, 0.0f, 3.0f)) { jm.Set("genres.Ambient.modScaleLow", ambient_modScaleLow); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleHigh", &ambient_modScaleHigh, 0.0f, 3.0f)) { jm.Set("genres.Ambient.modScaleHigh", ambient_modScaleHigh); changed = true; }
                if (ImGui::SliderFloat("Mod SmoothFactor", &ambient_modSmoothFactor, 0.01f, 2.0f)) { jm.Set("genres.Ambient.modSmoothFactor", ambient_modSmoothFactor); changed = true; }
                if (ImGui::SliderInt("Mod Span", &ambient_modSpan, 1, 32)) { jm.Set("genres.Ambient.modSpan", ambient_modSpan); changed = true; }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("EDM / Dance"))
            {
                if (ImGui::SliderFloat("Threshold LowRatio", &edm_thresholdLowRatio, 0.1f, 0.9f)) { jm.Set("genres.EDM.thresholdLowRatio", edm_thresholdLowRatio); changed = true; }
                if (ImGui::SliderFloat("Threshold BPM", &edm_thresholdBPM, 50.0f, 240.0f)) { jm.Set("genres.EDM.thresholdBPM", edm_thresholdBPM); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleX", &edm_modScaleX, 0.0f, 3.0f)) { jm.Set("genres.EDM.modScaleX", edm_modScaleX); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleY", &edm_modScaleY, 0.0f, 3.0f)) { jm.Set("genres.EDM.modScaleY", edm_modScaleY); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleLow", &edm_modScaleLow, 0.0f, 5.0f)) { jm.Set("genres.EDM.modScaleLow", edm_modScaleLow); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleHigh", &edm_modScaleHigh, 0.0f, 3.0f)) { jm.Set("genres.EDM.modScaleHigh", edm_modScaleHigh); changed = true; }
                if (ImGui::SliderFloat("Mod WavePeriod", &edm_modWavePeriod, 1.0f, 32.0f)) { jm.Set("genres.EDM.modWavePeriod", edm_modWavePeriod); changed = true; }
                if (ImGui::SliderFloat("Mod SmoothFactor", &edm_modSmoothFactor, 0.01f, 2.0f)) { jm.Set("genres.EDM.modSmoothFactor", edm_modSmoothFactor); changed = true; }
                if (ImGui::SliderInt("Mod Span", &edm_modSpan, 1, 16)) { jm.Set("genres.EDM.modSpan", edm_modSpan); changed = true; }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Melodic / Classical"))
            {
                if (ImGui::SliderFloat("Threshold MidRatio", &melodic_thresholdMidRatio, 0.1f, 0.9f)) { jm.Set("genres.Melodic.thresholdMidRatio", melodic_thresholdMidRatio); changed = true; }
                if (ImGui::SliderFloat("Threshold HighRatio", &melodic_thresholdHighRatio, 0.1f, 0.9f)) { jm.Set("genres.Melodic.thresholdHighRatio", melodic_thresholdHighRatio); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleX", &melodic_modScaleX, 0.0f, 3.0f)) { jm.Set("genres.Melodic.modScaleX", melodic_modScaleX); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleY", &melodic_modScaleY, 0.0f, 3.0f)) { jm.Set("genres.Melodic.modScaleY", melodic_modScaleY); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleLow", &melodic_modScaleLow, 0.0f, 3.0f)) { jm.Set("genres.Melodic.modScaleLow", melodic_modScaleLow); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleHigh", &melodic_modScaleHigh, 0.0f, 3.0f)) { jm.Set("genres.Melodic.modScaleHigh", melodic_modScaleHigh); changed = true; }
                if (ImGui::SliderFloat("Mod WavePeriod", &melodic_modWavePeriod, 1.0f, 32.0f)) { jm.Set("genres.Melodic.modWavePeriod", melodic_modWavePeriod); changed = true; }
                if (ImGui::SliderFloat("Mod SmoothFactor", &melodic_modSmoothFactor, 0.01f, 2.0f)) { jm.Set("genres.Melodic.modSmoothFactor", melodic_modSmoothFactor); changed = true; }
                if (ImGui::SliderInt("Mod Span", &melodic_modSpan, 1, 16)) { jm.Set("genres.Melodic.modSpan", melodic_modSpan); changed = true; }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Hard Rock / Metal"))
            {
                if (ImGui::SliderFloat("Threshold BPM", &metal_thresholdBPM, 50.0f, 240.0f)) { jm.Set("genres.Metal.thresholdBPM", metal_thresholdBPM); changed = true; }
                if (ImGui::SliderFloat("Threshold AvgAll", &metal_thresholdAvgAll, 0.005f, 0.5f)) { jm.Set("genres.Metal.thresholdAvgAll", metal_thresholdAvgAll); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleX", &metal_modScaleX, 0.0f, 3.0f)) { jm.Set("genres.Metal.modScaleX", metal_modScaleX); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleY", &metal_modScaleY, 0.0f, 3.0f)) { jm.Set("genres.Metal.modScaleY", metal_modScaleY); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleLow", &metal_modScaleLow, 0.0f, 3.0f)) { jm.Set("genres.Metal.modScaleLow", metal_modScaleLow); changed = true; }
                if (ImGui::SliderFloat("Mod ScaleHigh", &metal_modScaleHigh, 0.0f, 3.0f)) { jm.Set("genres.Metal.modScaleHigh", metal_modScaleHigh); changed = true; }
                if (ImGui::SliderFloat("Mod WavePeriod", &metal_modWavePeriod, 1.0f, 32.0f)) { jm.Set("genres.Metal.modWavePeriod", metal_modWavePeriod); changed = true; }
                if (ImGui::SliderFloat("Mod SmoothFactor", &metal_modSmoothFactor, 0.01f, 2.0f)) { jm.Set("genres.Metal.modSmoothFactor", metal_modSmoothFactor); changed = true; }
                if (ImGui::SliderInt("Mod Span", &metal_modSpan, 1, 16)) { jm.Set("genres.Metal.modSpan", metal_modSpan); changed = true; }
                ImGui::TreePop();
            }
        }

        if (changed)
        {
            jm.Save();
        }

        // 手動の「Regenerate Rail」ボタンのみで再生成（スライダー操作だけでは走らない）
        ImGui::Separator();
        if (ImGui::Button("Regenerate Rail"))
        {
            if (railManager)
            {
                railManager->Reset();
                GenerateStageFromAudio(currentSoundFilename, railManager);
            }
        }
    }
    else
    {
        ImGui::Text("Failed to load RailGenerator.json");
    }

    ImGui::End();
#endif
}