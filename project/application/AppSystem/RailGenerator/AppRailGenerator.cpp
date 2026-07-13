#include "Audio.h"
#include "OfflineAudioAnalyzer.h"
#include "../../Object/Rail/RailManager.h"
#include "AppRailGenerator.h"
#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

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
    if (!jm.Root().contains("clampX")) jm.Set("clampX", 50.0f);
    if (!jm.Root().contains("span")) jm.Set("span", 4);
    jm.Save();

    float advanceZ = jm.Get<float>("advanceZ", 20.0f);
    float smoothFactor = jm.Get<float>("smoothFactor", 1.0f);
    float scaleX = jm.Get<float>("scaleX", 400.0f);
    float scaleY = jm.Get<float>("scaleY", 150.0f);
    float clampX = jm.Get<float>("clampX", 50.0f);
    int span = jm.Get<int>("span", 4);

    // エンジンからWAVEの生データを取得する
    auto soundData = Audio::GetInstance()->GetSoundData(soundFilename);

    // エンジンに解析を依頼し、純粋なオーディオ特徴量の配列をもらう
    AudioAnalysisResult analysisData = OfflineAudioAnalyzer::Analyze(
        soundData.buffer.data(), static_cast<UINT32>(soundData.buffer.size()), soundData.wfex
    );

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

            // ゲーム独自のスケール調整
            float targetY = beat.rmsAll * scaleY;
            float targetX = (beat.rmsMid - 0.05f) * scaleX;

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