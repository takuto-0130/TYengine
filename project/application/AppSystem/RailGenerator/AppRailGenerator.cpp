#include "Audio.h"
#include "OfflineAudioAnalyzer.h"
#include "../../Object/Rail/RailManager.h"
#include "AppRailGenerator.h"

using namespace TYEngine::AudioSystem;

void GenerateStageFromAudio(const std::string& soundFilename, RailManager* railManager)
{
    // 1. エンジンからWAVEの生データを取得する
    // （※ GetSoundData は既存のBeatAnalyzer.cppで使われているものを想定）
    auto soundData = Audio::GetInstance()->GetSoundData(soundFilename);

    // 2. エンジンに解析を依頼し、純粋なオーディオ特徴量の配列をもらう
    AudioAnalysisResult analysisData = OfflineAudioAnalyzer::Analyze(
        soundData.buffer.data(), static_cast<UINT32>(soundData.buffer.size()), soundData.wfex
    );

    // 3. ゲーム固有のロジックでレール座標（制御点）に変換する
    std::vector<TYEngine::Utility::Vector3> newControlPoints;
    std::vector<bool> newTriggerFlags;

    float currentZ = 0.0f;
    const float ADVANCE_Z = 20.0f; // 1ビートで進む距離

    newControlPoints.push_back({ 0.0f, 0.0f, 0.0f });
    newTriggerFlags.push_back(false);

    int span = 4;

    int index = 0;

    for (const auto& beat : analysisData.beats)
    {
        if(index % span == 0)
        {
            currentZ += ADVANCE_Z;

            // ゲーム独自のスケール調整
            float targetY = beat.rmsAll * 150.0f;
            float targetX = (beat.rmsMid - 0.05f) * 400.0f;

            // 制限(クランプ)
            if (targetX > 50.0f) targetX = 50.0f;
            if (targetX < -50.0f) targetX = -50.0f;

            newControlPoints.push_back({ targetX, targetY, currentZ });
            // newTriggerFlags.push_back(true); // ビート位置に敵配置トリガーを置く等
        }
        index++;
    }

    // 4. 生成したデータを RailManager に直接セットして再構築
    railManager->SetDynamicData(newControlPoints, newTriggerFlags);
}