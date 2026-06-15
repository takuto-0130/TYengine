#pragma once
#include "AudioAnalysisResult.h"
#include <Windows.h>

namespace TYEngine
{
    namespace AudioSystem
    {
        class OfflineAudioAnalyzer
        {
        public:
            /// <summary>WAVEデータを解析し、ゲーム側で使いやすい特徴量配列を返す</summary>
            static AudioAnalysisResult Analyze(const BYTE* pData, UINT32 dataSize, const WAVEFORMATEX& wfx);
        };
    }
}