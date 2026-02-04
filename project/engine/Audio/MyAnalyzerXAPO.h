#pragma once
#include <xapobase.h>
#include <wrl.h>
#include <vector>

namespace TYEngine {
namespace Audio {

class Audio;

/// <summary>
/// カスタムXAPO（Cross-Platform Audio Processing Object）。
/// オーディオストリームに挿入し、解析用データ（波形、FFT）を取得する。
/// </summary>
class __declspec(uuid("2dde0a3b-45d5-4a48-a9e6-a3a8129ef91a"))
    MyAnalyzerXAPO : public Microsoft::WRL::RuntimeClass<
    Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
    Microsoft::WRL::FtmBase>, // IXAPO 等をここから削除
    public CXAPOParametersBase
{
private:
    friend class Audio;
    friend class Microsoft::WRL::Details::MakeAllocator<MyAnalyzerXAPO>;
public:
    // あいまいさを解決するため、IUnknown のメソッドを明示的に委譲する
    STDMETHOD(QueryInterface)(REFIID riid, _COM_Outptr_ void** ppvObject) override
    {
        return CXAPOParametersBase::QueryInterface(riid, ppvObject);
    }
    STDMETHOD_(ULONG, AddRef)() override
    {
        return CXAPOParametersBase::AddRef();
    }
    STDMETHOD_(ULONG, Release)() override
    {
        return CXAPOParametersBase::Release();
    }
    MyAnalyzerXAPO();
    ~MyAnalyzerXAPO();

    static const UINT32 FFT_SIZE = 1024;     ///< FFTサンプルサイズ
    static const UINT32 DELAY_FRAMES = 5;    ///< 遅延フレーム数
    static const UINT32 WAVEFORM_SIZE = 441; ///< 波形バッファサイズ

public:
    /// <summary>
    /// プロセス前のロック処理。
    /// フォーマットの確認やパラメータの更新を行う。
    /// </summary>
    HRESULT __stdcall LockForProcess(
        UINT32 inputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* inputLockedParameters,
        UINT32 outputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* outputLockedParameters
    ) override;

    /// <summary>
    /// オーディオ処理の実行。
    /// ストリームからデータをコピーし、解析用バッファに格納する。
    /// </summary>
    void __stdcall Process(
        UINT32 InputProcessParameterCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
        UINT32 OutputProcessParameterCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
        BOOL IsEnabled
    ) override;

    // getter
    float GetRMS() { return latestRMS_; }
    std::vector<float>& GetFFT() { return latestFFT_; }
    std::vector<float>& GetWaveform() { return latestWaveform_; }

private:
    void ComputeFFT();

private:
    /// <summary>入力チャンネル数。</summary>
    UINT32 channels_ = 0;

    /// <summary>遅延バッファ（時間差計測用）。</summary>
    std::vector<std::vector<float>> delayBuffer_;
    /// <summary>遅延バッファの書き込みインデックス。</summary>
    UINT32 delayIndex_ = 0;

    /// <summary>FFT入力バッファ。</summary>
    std::vector<float> fftInput_;
    /// <summary>FFT実部。</summary>
    std::vector<float> fftReal_;
    /// <summary>FFT虚部。</summary>
    std::vector<float> fftImag_;

    /// <summary>最新のRMS（音圧）値。</summary>
    float latestRMS_ = 0.0f;
    /// <summary>最新のFFT解析結果。</summary>
    std::vector<float> latestFFT_;
    /// <summary>最新の波形データ（UI表示用）。</summary>
    std::vector<float> latestWaveform_;  // タイムドメイン用
};

} // namespace Audio
} // namespace TYEngine
