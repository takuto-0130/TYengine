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

            BiquadFilter lowFilter;
            lowFilter.SetCoefficients(FilterType::LowPassFilter, (float)wfx.nSamplesPerSec, 150.0f, 0.707f);
            std::vector<float> lowData(numSamples, 0.0f);
            for (int i = 0; i < numSamples; ++i) lowData[i] = lowFilter.Process(audioData[i]);

            BiquadFilter midFilter;
            midFilter.SetCoefficients(FilterType::BandPassFilter, (float)wfx.nSamplesPerSec, 1000.0f, 1.0f);
            std::vector<float> midData(numSamples, 0.0f);
            for (int i = 0; i < numSamples; ++i) midData[i] = midFilter.Process(audioData[i]);

            BiquadFilter highFilter;
            highFilter.SetCoefficients(FilterType::HighPassFilter, (float)wfx.nSamplesPerSec, 4000.0f, 0.707f);
            std::vector<float> highData(numSamples, 0.0f);
            for (int i = 0; i < numSamples; ++i) highData[i] = highFilter.Process(audioData[i]);

            // 3. ビートごとに分割して特徴量（RMS）を計算し、配列に詰める
            float samplesPerBeat = (float)wfx.nSamplesPerSec * (60.0f / result.bpm);
            int beatCount = static_cast<int>((float)numSamples / samplesPerBeat);

            for (int b = 0; b < beatCount; ++b)
            {
                int startIndex = static_cast<int>(b * samplesPerBeat);
                int endIndex = static_cast<int>((b + 1) * samplesPerBeat);
                if (endIndex > numSamples) endIndex = numSamples;

                float sumAllSq = 0.0f, sumLowSq = 0.0f, sumMidSq = 0.0f, sumHighSq = 0.0f;
                int count = endIndex - startIndex;

                const float* pAudio = audioData.data();
                const float* pLow = lowData.data();
                const float* pMid = midData.data();
                const float* pHigh = highData.data();

                for (int i = startIndex; i < endIndex; ++i)
                {
                    sumAllSq += pAudio[i] * pAudio[i];
                    sumLowSq += pLow[i] * pLow[i];
                    sumMidSq += pMid[i] * pMid[i];
                    sumHighSq += pHigh[i] * pHigh[i];
                }

                BeatFeature feature;
                feature.timeSec = (float)startIndex / (float)wfx.nSamplesPerSec;
                feature.rmsAll = std::sqrt(sumAllSq / count);
                feature.rmsLow = std::sqrt(sumLowSq / count);
                feature.rmsMid = std::sqrt(sumMidSq / count);
                feature.rmsHigh = std::sqrt(sumHighSq / count);

                result.beats.push_back(feature);
            }

            // 曲全体の平均特徴量の算出
            if (!result.beats.empty())
            {
                float sumLow = 0.0f, sumMid = 0.0f, sumHigh = 0.0f, sumAll = 0.0f;
                for (const auto& beat : result.beats)
                {
                    sumLow += beat.rmsLow;
                    sumMid += beat.rmsMid;
                    sumHigh += beat.rmsHigh;
                    sumAll += beat.rmsAll;
                }
                float numBeats = static_cast<float>(result.beats.size());
                result.avgLow = sumLow / numBeats;
                result.avgMid = sumMid / numBeats;
                result.avgHigh = sumHigh / numBeats;
                result.avgAll = sumAll / numBeats;
            }

            return result;
        }
    }
}