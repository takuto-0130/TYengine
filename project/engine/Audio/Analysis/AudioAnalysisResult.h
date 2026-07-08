#pragma once
#include <vector>

namespace TYEngine
{
    namespace AudioSystem
    {
        /// <summary>1ビート（または一定時間）ごとのオーディオ特徴量</summary>
        struct BeatFeature
        {
            float timeSec;  // 曲の開始からの時間（秒）
            float rmsAll;   // 全帯域の音量（音圧）
            float rmsMid;   // 中音域の強さ（メロディ等）
        };

        /// <summary>オフライン解析の全体結果</summary>
        struct AudioAnalysisResult
        {
            float bpm;      // 検出されたBPM
            std::vector<BeatFeature> beats; // 曲全体のビートデータ配列
        };
    }
}