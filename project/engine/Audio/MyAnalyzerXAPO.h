#pragma once
#include <xapobase.h>
#include <vector>

class __declspec(uuid("2dde0a3b-45d5-4a48-a9e6-a3a8129ef91a"))
    MyAnalyzerXAPO : public CXAPOParametersBase
{
public:
    static const UINT32 FFT_SIZE = 1024;
    static const UINT32 DELAY_FRAMES = 5;

public:
    MyAnalyzerXAPO();
    ~MyAnalyzerXAPO();

    HRESULT __stdcall LockForProcess(
        UINT32 inputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* inputLockedParameters,
        UINT32 outputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* outputLockedParameters
    ) override;

    void __stdcall Process(
        UINT32 InputProcessParameterCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
        UINT32 OutputProcessParameterCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
        BOOL IsEnabled
    ) override;

    float latestRMS = 0.0f;
    std::vector<float> latestFFT;

private:
    void computeFFT();

private:
    UINT32 channels = 0;

    std::vector<std::vector<float>> delayBuffer;
    UINT32 delayIndex = 0;

    std::vector<float> fftInput;
    std::vector<float> fftReal;
    std::vector<float> fftImag;
};
