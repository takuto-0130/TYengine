#include "MyAnalyzerXAPO.h"
#include <cmath>
#include <cstring>

// ---------------------------------------------------------
//  XAPO の登録プロパティ
// ---------------------------------------------------------
static XAPO_REGISTRATION_PROPERTIES regProps =
{
    __uuidof(MyAnalyzerXAPO),      // CLSID
    L"MyAnalyzerXAPO",             // FriendlyName
    L"Copyright (C) 2025",
    1,                             // MajorVersion
    0,                             // MinorVersion

    // フラグ構成（入力/出力フォーマットの整合性を厳格にする）
    XAPO_FLAG_CHANNELS_MUST_MATCH |
    XAPO_FLAG_FRAMERATE_MUST_MATCH |
    XAPO_FLAG_BITSPERSAMPLE_MUST_MATCH |
    XAPO_FLAG_BUFFERCOUNT_MUST_MATCH |
    XAPO_FLAG_INPLACE_SUPPORTED,

    1, 1,  // Input buffer count MIN/MAX
    1, 1   // Output buffer count MIN/MAX
};

// ---------------------------------------------------------
//  コンストラクタ
//  - 本 XAPO はパラメータ無しのため pParameterBlocks=nullptr
// ---------------------------------------------------------
MyAnalyzerXAPO::MyAnalyzerXAPO()
    : CXAPOParametersBase(&regProps, nullptr, 0, TRUE)
{}

MyAnalyzerXAPO::~MyAnalyzerXAPO()
{}

// ---------------------------------------------------------
//  LockForProcess
//  - XAudio2 が XAPO を使用する直前に呼ばれる
//  - 入力フォーマット(チャンネル数/bit深度等) を記録
// ---------------------------------------------------------
HRESULT __stdcall MyAnalyzerXAPO::LockForProcess(
    UINT32 inputLockedParameterCount,
    const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* inputLockedParameters,
    UINT32 outputLockedParameterCount,
    const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* outputLockedParameters)
{
    // ベースクラスによる基本チェック
    HRESULT hr = CXAPOParametersBase::LockForProcess(
        inputLockedParameterCount,
        inputLockedParameters,
        outputLockedParameterCount,
        outputLockedParameters
    );
    if (FAILED(hr))
        return hr;

    const WAVEFORMATEX* fmt = inputLockedParameters[0].pFormat;

    channels = fmt->nChannels;
    bytesPerSample = fmt->wBitsPerSample / 8;

    // この XAPO は 32bit float 専用
    if (fmt->wFormatTag != WAVE_FORMAT_IEEE_FLOAT || fmt->wBitsPerSample != 32)
    {
        // XAudio2の自動変換に依存しない場合はエラーにしてもよい
        // return E_INVALIDARG;
    }

    return S_OK;
}

// ---------------------------------------------------------
//  Process
//  - 毎フレーム呼ばれる音声処理
//  - 入力バッファから RMS を計算
//  - 出力バッファはインプレース（またはコピー）で処理
// ---------------------------------------------------------
void __stdcall MyAnalyzerXAPO::Process(
    UINT32 InputProcessParameterCount,
    const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
    UINT32 OutputProcessParameterCount,
    XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
    BOOL IsEnabled)
{
    UNREFERENCED_PARAMETER(InputProcessParameterCount);
    UNREFERENCED_PARAMETER(OutputProcessParameterCount);

    // XAPO が無効化されている場合
    if (!IsEnabled)
        return;

    const void* pIn = pInputProcessParameters[0].pBuffer;
    void* pOut = pOutputProcessParameters[0].pBuffer;
    UINT32 frameCount = pInputProcessParameters[0].ValidFrameCount;

    UINT32 totalSamples = frameCount * channels;
    UINT32 bufferBytes = totalSamples * bytesPerSample;

    // インプレース or コピー
    if (pIn != pOut)
    {
        memcpy(pOut, pIn, bufferBytes);
    }

    // 32-bit float 前提
    if (bytesPerSample != sizeof(float))
    {
        latestRMS = 0.0f;
        return;
    }

    // RMS 計算
    const float* samples = reinterpret_cast<const float*>(pIn);

    double sumSq = 0.0;
    for (UINT32 i = 0; i < totalSamples; i++)
    {
        float s = samples[i];
        sumSq += s * s;
    }

    latestRMS = (totalSamples > 0)
        ? static_cast<float>(std::sqrt(sumSq / totalSamples))
        : 0.0f;
}

