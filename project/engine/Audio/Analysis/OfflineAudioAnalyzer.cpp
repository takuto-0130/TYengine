#include "OfflineAudioAnalyzer.h"
#include "BPMDetector.h"
#include "BiquadFilter.h"
#include <cmath>

namespace TYEngine
{
    namespace AudioSystem
    {
        AudioAnalysisResult OfflineAudioAnalyzer::Analyze(const BYTE* pData, UINT32 dataSize, const WAVEFORMATEX& wfx)
        {
            AudioAnalysisResult result;

            // 1. BPM解析
            result.bpm = BPMDetector::AnalyzeBPM(pData, dataSize, wfx);
            if (result.bpm <= 0.0f) result.bpm = 120.0f;

            // 2. モノラルfloatへの変換とMid帯域の抽出
            int numSamples = dataSize / wfx.nBlockAlign;
            std::vector<float> audioData(numSamples, 0.0f);

            int channels = wfx.nChannels;
            const INT16* pSample = reinterpret_cast<const INT16*>(pData);
            for (int i = 0; i < numSamples; ++i)
            {
                float sum = 0.0f;
                for (int c = 0; c < channels; ++c) sum += pSample[i * channels + c] / 32768.0f;
                audioData[i] = sum / channels;
            }

            BiquadFilter midFilter;
            midFilter.SetCoefficients(FilterType::BandPassFilter, (float)wfx.nSamplesPerSec, 1000.0f, 1.0f);
            std::vector<float> midData(numSamples, 0.0f);
            for (int i = 0; i < numSamples; ++i) midData[i] = midFilter.Process(audioData[i]);

            // 3. ビートごとに分割して特徴量（RMS）を計算し、配列に詰める
            float samplesPerBeat = (float)wfx.nSamplesPerSec * (60.0f / result.bpm);
            int beatCount = static_cast<int>((float)numSamples / samplesPerBeat);

            for (int b = 0; b < beatCount; ++b)
            {
                int startIndex = static_cast<int>(b * samplesPerBeat);
                int endIndex = static_cast<int>((b + 1) * samplesPerBeat);
                if (endIndex > numSamples) endIndex = numSamples;

                float sumAllSq = 0.0f, sumMidSq = 0.0f;
                int count = endIndex - startIndex;

                const float* pAudio = audioData.data();
                const float* pMid = midData.data();

                for (int i = startIndex; i < endIndex; ++i)
                {
                    sumAllSq += pAudio[i] * pAudio[i];
                    sumMidSq += pMid[i] * pMid[i];
                }

                BeatFeature feature;
                feature.timeSec = (float)startIndex / (float)wfx.nSamplesPerSec;
                feature.rmsAll = std::sqrt(sumAllSq / count);
                feature.rmsMid = std::sqrt(sumMidSq / count);

                result.beats.push_back(feature);
            }

            return result;
        }
    }
}