#include "MyAnalyzerXAPO.h"
#include <cmath>
#include <cstring>

// -----------------------------------------
// XAPO の登録プロパティ
// -----------------------------------------
static XAPO_REGISTRATION_PROPERTIES regProps =
{
    __uuidof(MyAnalyzerXAPO),
    L"MyAnalyzerXAPO",
    L"Copyright (C) 2025",
    1, 0,    // major / minor version
    XAPO_FLAG_CHANNELS_MUST_MATCH |
    XAPO_FLAG_FRAMERATE_MUST_MATCH |
    XAPO_FLAG_BITSPERSAMPLE_MUST_MATCH |
    XAPO_FLAG_BUFFERCOUNT_MUST_MATCH |
    XAPO_FLAG_INPLACE_SUPPORTED,
    1, 1,    // min / max input
    1, 1     // min / max output
};

// -----------------------------------------
// コンストラクタ
// -----------------------------------------
MyAnalyzerXAPO::MyAnalyzerXAPO()
    : CXAPOParametersBase(&regProps, nullptr, 0, TRUE)   // fProducer = TRUE
{}

MyAnalyzerXAPO::~MyAnalyzerXAPO()
{}

// -----------------------------------------
// LockForProcess（フォーマット情報を受け取る）
// -----------------------------------------
HRESULT __stdcall MyAnalyzerXAPO::LockForProcess(
    UINT32 inputLockedParameterCount,
    const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* inputLockedParameters,
    UINT32 outputLockedParameterCount,
    const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* outputLockedParameters)
{
    UNREFERENCED_PARAMETER(inputLockedParameterCount);
    UNREFERENCED_PARAMETER(outputLockedParameterCount);

    const WAVEFORMATEX* fmt = inputLockedParameters[0].pFormat;

    channels = fmt->nChannels;
    bytesPerSample = fmt->wBitsPerSample / 8;

    return CXAPOParametersBase::LockForProcess(
        inputLockedParameterCount,
        inputLockedParameters,
        outputLockedParameterCount,
        outputLockedParameters
    );
}

// -----------------------------------------
// Process（生 IXAPO 形式）
// ここで PCM を処理・コピーして解析する
// -----------------------------------------
void __stdcall MyAnalyzerXAPO::Process(
    UINT32 InputProcessParameterCount,
    const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
    UINT32 OutputProcessParameterCount,
    XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
    BOOL IsEnabled)
{
    UNREFERENCED_PARAMETER(InputProcessParameterCount);
    UNREFERENCED_PARAMETER(OutputProcessParameterCount);

    if (!IsEnabled)
        return;

    const void* pInBuffer = pInputProcessParameters->pBuffer;
    void* pOutBuffer = pOutputProcessParameters->pBuffer;
    UINT32      frameCount = pInputProcessParameters->ValidFrameCount;

    // --------------------------------------------------
    // スルー処理（in-place 対応）
    // --------------------------------------------------
    if (pInBuffer != pOutBuffer)
    {
        memcpy(
            pOutBuffer,
            pInBuffer,
            frameCount * channels * sizeof(float)
        );
    }

    // --------------------------------------------------
    // ★解析処理：RMS（音量）を計算
    // --------------------------------------------------
    const float* samples = reinterpret_cast<const float*>(pInBuffer);

    double sum = 0.0;
    for (UINT32 i = 0; i < frameCount * channels; i++)
    {
        float s = samples[i];
        sum += s * s;
    }

    latestRMS = static_cast<float>(
        sqrt(sum / (frameCount * channels))
        );
}
