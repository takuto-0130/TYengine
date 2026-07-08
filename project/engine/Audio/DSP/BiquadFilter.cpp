#include "BiquadFilter.h"

// "RBJ Audio EQ Cookbook" の計算式

namespace TYEngine
{
    namespace AudioSystem
    {
        void BiquadFilter::SetCoefficients(FilterType type, float sampleRate, float frequency, float q, float dbGain)
        {
            float A = 0.0f;
            if(type == FilterType::Peaking || type == FilterType::LowShelf || type == FilterType::HighShelf)
            {
                A = std::powf(10.0f, dbGain / 40.0f);
            }
            else
            {
                A = std::sqrtf(std::powf(10.0f, dbGain / 40.0f));
            }
            float w0 = 2.0f * std::numbers::pi_v<float> *frequency / sampleRate;
            float alpha = std::sinf(w0) / (2.0f * q);
            float cosw0 = std::cosf(w0);

            switch (type)
            {
            case FilterType::LowPassFilter:
                b0_ = (1.0f - cosw0) / 2.0f;
                b1_ = 1.0f - cosw0;
                b2_ = (1.0f - cosw0) / 2.0f;
                a0_ = 1.0f + alpha;
                a1_ = -2.0f * cosw0;
                a2_ = 1.0f - alpha;
                break;

            case FilterType::HighPassFilter:
                b0_ = (1.0f + cosw0) / 2.0f;
                b1_ = -(1.0f + cosw0);
                b2_ = (1.0f + cosw0) / 2.0f;
                a0_ = 1.0f + alpha;
                a1_ = -2.0f * cosw0;
                a2_ = 1.0f - alpha;
                break;

            case FilterType::BandPassFilter:
                b0_ = alpha;
                b1_ = 0;
                b2_ = -alpha;
                a0_ = 1.0f + alpha;
                a1_ = -2.0f * cosw0;
                a2_ = 1.0f - alpha;
                break;

            case FilterType::Peaking:
                b0_ = 1.0f + alpha * A;
                b1_ = -2.0f * cosw0;
                b2_ = 1.0f - alpha * A;
                a0_ = 1.0f + alpha / A;
                a1_ = -2.0f * cosw0;
                a2_ = 1.0f - alpha / A;
                break;

            case FilterType::LowShelf:
                b0_ = A * ((A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * std::sqrtf(A) * alpha);
                b1_ = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosw0);
                b2_ = A * ((A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * std::sqrtf(A) * alpha);
                a0_ = (A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * std::sqrtf(A) * alpha;
                a1_ = -2.0f * ((A - 1.0f) + (A + 1.0f) * cosw0);
                a2_ = (A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * std::sqrtf(A) * alpha;
                break;

            case FilterType::HighShelf:
                b0_ = A * ((A + 1.0f) + (A - 1.0f) * cosw0 + 2.0f * std::sqrtf(A) * alpha);
                b1_ = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosw0);
                b2_ = A * ((A + 1.0f) + (A - 1.0f) * cosw0 - 2.0f * std::sqrtf(A) * alpha);
                a0_ = (A + 1.0f) - (A - 1.0f) * cosw0 + 2.0f * std::sqrtf(A) * alpha;
                a1_ = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosw0);
                a2_ = (A + 1.0f) - (A - 1.0f) * cosw0 - 2.0f * std::sqrtf(A) * alpha;
                break;
            }

            // 処理を高速化するため、あらかじめ a0 で割って正規化しておく
            b0_ /= a0_;
            b1_ /= a0_;
            b2_ /= a0_;
            a1_ /= a0_;
            a2_ /= a0_;
        }

        float BiquadFilter::Process(float input)
        {
            // 差分方程式: y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
            float output = b0_ * input + b1_ * in1_ + b2_ * in2_ - a1_ * out1_ - a2_ * out2_;

            // 履歴の更新
            in2_ = in1_;
            in1_ = input;
            out2_ = out1_;
            out1_ = output;

            return output;
        }

        void BiquadFilter::Reset()
        {
            in1_ = in2_ = out1_ = out2_ = 0.0f;
        }
    }
}