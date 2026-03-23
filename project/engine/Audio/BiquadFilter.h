#pragma once
#include <cmath>
#include <numbers>

namespace TYEngine
{
    namespace AudioSystem
    {
        enum class FilterType
        {
            LowPassFilter,
            HighPassFilter,
            LowShelf,
            HighShelf,
            Peaking
        };

        class BiquadFilter
        {
        public:
            BiquadFilter() = default;

            // フィルタのパラメータを設定する
            // sampleRate: サンプリングレート (例: 48000)
            // frequency: 対象となる周波数Hz (例: 低音なら100, 高音なら8000など)
            // q: Q値 (帯域の広さ。通常は 0.707)
            // dbGain: ブースト/カットする量（デシベル。プラスで強調、マイナスで減衰）
            void SetCoefficients(FilterType type, float sampleRate, float frequency, float q, float dbGain);

            // 1サンプル分の処理を行う
            float Process(float input);

            // 履歴をクリア（曲の切り替わりやシーク時に呼ぶ）
            void Reset();

        private:
            // 係数
            float a0_ = 1.0f, a1_ = 0.0f, a2_ = 0.0f;
            float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;

            // 過去の入出力履歴
            float in1_ = 0.0f, in2_ = 0.0f; // 過去の入力
            float out1_ = 0.0f, out2_ = 0.0f; // 過去の出力
        };
    }
}