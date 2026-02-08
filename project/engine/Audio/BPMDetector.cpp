#include "BPMDetector.h"
#include <cmath>
#include <algorithm>
#include <vector>

namespace TYEngine
{
    namespace AudioSystem
    {
        float BPMDetector::AnalyzeBPM(const BYTE* pData, UINT32 dataSize, const WAVEFORMATEX& wfx)
        {
            // 1. フォーマットチェック (16bit PCMのみ対応の簡易版)
            if (wfx.wBitsPerSample != 16) return 0.0f; // 24bit/32bit対応が必要なら後に拡張

            // 2. PCMデータを float モノラルに変換 & 縮小 (ダウンサンプリング)
            //    解析精度のため、元の44100Hzのままではなく、
            //    「音量エンベロープ」として扱いやすいレート(例: 約100Hz相当)に圧縮します。

            // 1ブロックあたりのサンプル数 (44100Hzなら1024サンプル≒约43Hz)
            // ここを細かくすると解析精度が上がり計算が重くなる
            const int BLOCK_SIZE = 512;

            int numSamples = dataSize / wfx.nBlockAlign;
            int numBlocks = numSamples / BLOCK_SIZE;

            // ガード1: データが少なすぎる（例えば0.5秒以下）ならBPM解析不能として即リターン
            // (BPM 200でも1拍0.3秒、最低でも数拍分はないと解析できません)
            if (numBlocks < 50)
            {
                return 0.0f;
            }

            std::vector<float> envelope;
            envelope.reserve(numBlocks);

            const short* pSample16 = reinterpret_cast<const short*>(pData);
            int channels = wfx.nChannels;

            for (int i = 0; i < numBlocks; ++i)
            {
                float sumEnergy = 0.0f;
                for (int j = 0; j < BLOCK_SIZE; ++j)
                {
                    int index = (i * BLOCK_SIZE + j) * channels;

                    // ステレオならL+Rを合成
                    float sampleVal = (float)pSample16[index]; // L
                    if (channels == 2) sampleVal += (float)pSample16[index + 1]; // R

                    // エネルギー（絶対値や二乗）を加算
                    sumEnergy += std::abs(sampleVal);
                }
                envelope.push_back(sumEnergy);
            }

            // 3. 全体自己相関 (Autocorrelation)
            //    曲全体を使って「最も重なり合う周期」を探す

            float maxCorrelation = 0.0f;
            int bestLag = 0;

            // 探索範囲: BPM 60 ～ 240
            // サンプリングレートは (wfx.nSamplesPerSec / BLOCK_SIZE)
            float envelopeRate = (float)wfx.nSamplesPerSec / (float)BLOCK_SIZE;

            int minLag = (int)(envelopeRate * (60.0f / 240.0f)); // BPM 240
            int maxLag = (int)(envelopeRate * (60.0f / 60.0f));  // BPM 60

            // ガード2: エンベロープ長が探索範囲より短い場合もスキップ
            // ここで (envelope.size() - lag) がアンダーフローするのを防ぐ
            if (envelope.size() <= (size_t)maxLag)
            {
                return 0.0f;
            }

            for (int lag = minLag; lag <= maxLag; ++lag)
            {
                double correlation = 0.0;

                // 高速化のため、間引いて計算しても良いが、ここは全データで計算
                for (size_t i = 0; i < envelope.size() - lag; ++i)
                {
                    correlation += envelope[i] * envelope[i + lag];
                }

                // 正規化はここでは省略可能（同じデータ長での比較なので）
                // ただし、ラグが短いほうがデータ点数が増えて有利になるのを防ぐ補正は必要
                // (データ数で割る = 平均をとる)
                correlation /= (double)(envelope.size() - lag);

                if (correlation > maxCorrelation)
                {
                    maxCorrelation = (float)correlation;
                    bestLag = lag;
                }
            }

            // 4. 結果をBPMに変換
            if (bestLag > 0)
            {
                float beatSec = (float)bestLag / envelopeRate;
                float bpm = 60.0f / beatSec;

                // ハーフテンポ（倍取り）問題の補正
                // ゲーム用途なら 100～200 BPM が多いため、
                // 90未満が出たら「ハーフテンポ誤検知」とみなして2倍にするのが安全です。

                // 例: 84 BPM -> 168 BPM に変換
                // 例: 70 BPM -> 140 BPM に変換
                while (bpm < 90.0f)
                {
                    bpm *= 2.0f;
                }

                // 逆に速すぎる場合（300以上など）のガードも入れておくと安全
                while (bpm > 240.0f)
                {
                    bpm /= 2.0f;
                }

                return bpm;
            }

            return 0.0f;
        }
    }
}