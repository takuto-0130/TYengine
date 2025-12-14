#include "MyAnalyzerXAPO.h"
#include <cmath>
#include <cstring>
#include <numbers>

// XAPO registration
static XAPO_REGISTRATION_PROPERTIES regProps =
{
    __uuidof(MyAnalyzerXAPO),
    L"MyAnalyzerXAPO",
    L"TYengine",
    1, 0,
    XAPO_FLAG_CHANNELS_MUST_MATCH |
    XAPO_FLAG_FRAMERATE_MUST_MATCH |
    XAPO_FLAG_BUFFERCOUNT_MUST_MATCH |
    XAPO_FLAG_INPLACE_SUPPORTED,
    1, 1,
    1, 1
};

// --------------------------------------------------------------
MyAnalyzerXAPO::MyAnalyzerXAPO()
    : CXAPOParametersBase(&regProps, nullptr, 0, TRUE)
{
    latestFFT.resize(FFT_SIZE, 0.0f);
    latestWaveform.resize(WAVEFORM_SIZE, 0.0f);

    delayBuffer.resize(DELAY_FRAMES);
    for (auto& v : delayBuffer)
        v.resize(FFT_SIZE, 0.0f);

    fftInput.resize(FFT_SIZE);
    fftReal.resize(FFT_SIZE);
    fftImag.resize(FFT_SIZE);
}

MyAnalyzerXAPO::~MyAnalyzerXAPO() {}

// --------------------------------------------------------------
HRESULT __stdcall MyAnalyzerXAPO::LockForProcess(
    UINT32, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* in_params,
    UINT32, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS*)
{
    const WAVEFORMATEX* fmt = in_params[0].pFormat;

    channels = fmt->nChannels;

    // XAudio2 は Submix で float32 に自動変換する
    // ここでは float32 前提で扱う
    return S_OK;
}

// --------------------------------------------------------------
// FFT (Cooley–Tukey)
// --------------------------------------------------------------
void MyAnalyzerXAPO::ComputeFFT()
{
    for (UINT32 i = 0; i < FFT_SIZE; i++)
    {
        fftReal[i] = fftInput[i];
        fftImag[i] = 0.0f;
    }

    UINT32 j = 0;
    for (UINT32 i = 0; i < FFT_SIZE; i++)
    {
        if (i < j)
        {
            std::swap(fftReal[i], fftReal[j]);
            std::swap(fftImag[i], fftImag[j]);
        }
        UINT32 bit = FFT_SIZE >> 1;
        while (j & bit) { j ^= bit; bit >>= 1; }
        j |= bit;
    }

    for (UINT32 len = 2; len <= FFT_SIZE; len <<= 1)
    {
        float ang = -2.0f * std::numbers::pi_v<float> / len;
        float wcos = cosf(ang);
        float wsin = sinf(ang);

        for (UINT32 i = 0; i < FFT_SIZE; i += len)
        {
            float ucos = 1.0f;
            float usin = 0.0f;

            for (UINT32 k = 0; k < len / 2; k++)
            {
                UINT32 a = i + k;
                UINT32 b = i + k + len / 2;

                float xr = fftReal[b] * ucos - fftImag[b] * usin;
                float xi = fftReal[b] * usin + fftImag[b] * ucos;

                fftReal[b] = fftReal[a] - xr;
                fftImag[b] = fftImag[a] - xi;

                fftReal[a] += xr;
                fftImag[a] += xi;

                float ucos2 = ucos * wcos - usin * wsin;
                usin = ucos * wsin + usin * wcos;
                ucos = ucos2;
            }
        }
    }

    for (UINT32 i = 0; i < FFT_SIZE; i++)
        latestFFT[i] = sqrtf(fftReal[i] * fftReal[i] + fftImag[i] * fftImag[i]);
}

// --------------------------------------------------------------
void __stdcall MyAnalyzerXAPO::Process(
    UINT32,
    const XAPO_PROCESS_BUFFER_PARAMETERS* in,
    UINT32,
    XAPO_PROCESS_BUFFER_PARAMETERS* out,
    BOOL enabled)
{
    if (!enabled) return;

    const float* pIn = (const float*)in->pBuffer;
    float* pOut = (float*)out->pBuffer;
    UINT32 frameCount = in->ValidFrameCount;

    int copyCount = min(frameCount, (UINT32)latestWaveform.size());
    memcpy(latestWaveform.data(), pIn, sizeof(float) * copyCount);

    // ---- スルー処理 ----
    if (pIn != pOut) memcpy(pOut, pIn, frameCount * channels * sizeof(float));

    if (frameCount == 0) return;

    // ---- RMS ----
    double sum = 0.0;
    for (UINT32 i = 0; i < frameCount * channels; i++) sum += pIn[i] * pIn[i];

    latestRMS = (float)sqrt(sum / (frameCount * channels));

    // ---- 無音判定 ----
    // frameCount * channels 分の平均をとる
    if (latestRMS < 0.00001f)   // しきい値
    {
        // 無音なら FFT と delayBuffer をクリア
        std::fill(latestFFT.begin(), latestFFT.end(), 0.0f);

        for (auto& buf : delayBuffer) std::fill(buf.begin(), buf.end(), 0.0f);

        // 次のフレームのため fftInput もクリア
        std::fill(fftInput.begin(), fftInput.end(), 0.0f);

        return;   // これ以上処理しない
    }

    // ---- 遅延バッファ ----
    UINT32 copy = (frameCount < FFT_SIZE) ? frameCount : FFT_SIZE;

    memset(delayBuffer[delayIndex].data(), 0, FFT_SIZE * sizeof(float));
    memcpy(delayBuffer[delayIndex].data(), pIn, copy * sizeof(float));

    delayIndex = (delayIndex + 1) % DELAY_FRAMES;
    UINT32 readIndex = (delayIndex + DELAY_FRAMES - 1) % DELAY_FRAMES;

    memcpy(fftInput.data(), delayBuffer[readIndex].data(), FFT_SIZE * sizeof(float));

    ComputeFFT();
}
