#pragma once
#include <xaudio2.h>
#include <xapobase.h>

// ================================================================
//  MyAnalyzerXAPO
//  - 32bit float PCM の入力ストリームに対して RMS を計算する XAPO
//  - XAPO_FLAG_INPLACE_SUPPORTED により、インプレース処理が可能
//  - XAPOParametersBase を継承しているが、本XAPOではパラメータ無し
// ================================================================
class __declspec(uuid("2dde0a3b-45d5-4a48-a9e6-a3a8129ef91a"))
    MyAnalyzerXAPO : public CXAPOParametersBase
{
public:
    MyAnalyzerXAPO();
    ~MyAnalyzerXAPO();

    // XAudio2 が LockForProcess を呼び出す際に、
    // 使用されるフォーマット情報を取得して内部状態をセットする
    HRESULT __stdcall LockForProcess(
        UINT32 inputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* inputLockedParameters,
        UINT32 outputLockedParameterCount,
        const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* outputLockedParameters
    ) override;

    // 毎フレーム呼ばれる DSP 本体処理
    // - 入力バッファから RMS を計算
    // - 出力バッファへインプレースまたはコピー処理を行う
    void __stdcall Process(
        UINT32 InputProcessParameterCount,
        const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
        UINT32 OutputProcessParameterCount,
        XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
        BOOL IsEnabled
    ) override;

    // 最新の RMS 値 (外部から参照)
    float latestRMS = 0.0f;

private:
    UINT32 channels = 0;        // 入力チャンネル数
    UINT32 bytesPerSample = 0;  // 1サンプルあたりのバイト数（通常 4）
};

