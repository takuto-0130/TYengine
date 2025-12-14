#pragma once
#include <vector>
#include <array>
#include <imgui.h>

class AudioAnalyzer
{
public:
    static const int FFT_SIZE = 1024;
    static const int BANDS = 8;
    static const int RMS_HISTORY_SIZE = 120; // 2秒
    static const int DELAY_FRAMES = 5;

public:
    AudioAnalyzer();
    void Update();          // 全処理
    void Draw();            // ImGui描画

    // 値の取得（ゲーム側で利用）
    float GetSyncedRMS() const { return syncedRMS_; }
    const std::vector<float>& GetSmoothedSpectrum() const { return spectrumSmoothed_; }

    // 低域・中域・高域のグレースケール値（0〜1）
    float GetLowGray()  const { return lowGray_; }
    float GetMidGray()  const { return midGray_; }
    float GetHighGray() const { return highGray_; }

private:
    // 内部処理
    void UpdateRMS();
    void UpdateFFT();
    void UpdateWaveform();
    void UpdateSpectrumSmoothing();

    // Low/Mid/High グレースケール更新
    void UpdateBandGrayscale();

    std::vector<float> MakeLogSpectrum(
        const std::vector<float>& fft,
        int sampleRate,
        int bands);

    struct BandInfo
    {
        float low;
        float high;
        float center;
    };
    std::vector<BandInfo> CalcLogBands(int bands, float sampleRate);

    void DrawRSM(float width);
    void DrawSpectrum(float width);
    void DrawWaveform(float width);

private:
    // ---- RMS 関連 ----
    float rmsHistory_[RMS_HISTORY_SIZE] = {};
    int rmsIndex_ = 0;

    float rmsDelay_[DELAY_FRAMES] = {};
    int rmsDelayIndex_ = 0;

    float syncedRMS_ = 0.0f;

    // ---- FFT 関連 ----
    std::array<std::array<float, FFT_SIZE>, DELAY_FRAMES> fftDelay_;
    int fftDelayIndex_ = 0;

    std::vector<float> waveform_;  // UI 用の波形
    std::vector<float> waveformScroll_;  // スクロール用のリングバッファ
    int waveformWriteIndex_ = 0;

    // ---- スペクトラム ----
    std::vector<float> spectrumSmoothed_;

    // グレースケール値（0〜1）
    float lowGray_ = 0.0f;
    float midGray_ = 0.0f;
    float highGray_ = 0.0f;
};
