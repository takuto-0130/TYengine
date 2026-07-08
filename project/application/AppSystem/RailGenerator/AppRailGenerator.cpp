#include "Audio.h"
#include "OfflineAudioAnalyzer.h"
#include "../../Object/Rail/RailManager.h"
#include "AppRailGenerator.h"

using namespace TYEngine::AudioSystem;

void GenerateStageFromAudio(const std::string& soundFilename, RailManager* railManager)
{
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
    const float ADVANCE_Z = 20.0f; // 1ビートで進む距離

    newControlPoints.push_back({ 0.0f, 0.0f, 0.0f });
    newTriggerFlags.push_back(false);

    int span = 4;
    int index = 0;

    // 前回の平滑化された座標を保持する変数
    // スタート地点に合わせて 0.0f で初期化しておく
    float lastSmoothedX = 0.0f;
    float lastSmoothedY = 0.0f;

    // 平滑化の度合いを決める係数 (0.0f ～ 1.0f)
    // 1.0f だと平滑化なし（元のまま）。
    const float SMOOTH_FACTOR = 1.0f;

    for (const auto& beat : analysisData.beats)
    {
        if (index % span == 0)
        {
            currentZ += ADVANCE_Z;

            // ゲーム独自のスケール調整
            float targetY = beat.rmsAll * 150.0f;
            float targetX = (beat.rmsMid - 0.05f) * 400.0f;

            // 制限(クランプ)
            if (targetX > 50.0f) targetX = 50.0f;
            if (targetX < -50.0f) targetX = -50.0f;

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