#pragma once
#include "AudioSystemBase.h"
#include "AudioAnalyzer.h"
#include "BeatAnalyzer.h"
#include "SingletonObject.h"
#include "CriticalHealthAudioEffect.h"

/// <summary>
/// サウンドのカテゴリ定義。
/// BGM, SE, UIなどの種類を管理する。
/// </summary>
enum class SoundCategory
{
    BGM,    ///< BGM（背景音楽）
    SE,     ///< SE（効果音）
    UI,     ///< UI音（決定音、キャンセル音など）
    CategoryNum
};

/// <summary>
/// ゲーム内オーディオ管理の具体的な実装クラス。
/// AudioSystemBase を継承し、BGM, SE, UI の3カテゴリを登録する。
/// </summary>
class GameAudio :
    public TYEngine::AudioSystem::AudioSystemBase<SoundCategory>,
    public TYEngine::Utility::SingletonObject<GameAudio>
{
    friend class SingletonObject<GameAudio>;
    friend struct std::default_delete<GameAudio>;

private:
    // 外部からの new/delete を禁止
    GameAudio() = default;
    ~GameAudio() = default;

    /// <summary>
    /// 初期ロード時に呼び出される。
    /// BGM, SE, UI の各カテゴリを作成・登録する。
    /// </summary>
    void OnInit() override;

public:
    void Update();

    void InitBeatAnalyzer(const std::string& filename, SoundCategory soundCategory = SoundCategory::BGM);

    void Stop(int resourceNum)
    {
        GetAudio()->Stop(resourceNum);
    }
    void Pause(int resourceNum)
    {
        GetAudio()->Pause(resourceNum);
    }
    void Resume(int resourceNum)
    {
        GetAudio()->Resume(resourceNum);
    }

    void Pitch(int resourceNum, float pitch)
    {
        GetAudio()->Pitch(resourceNum, pitch);
    }

    TYEngine::AudioSystem::BeatAnalyzer& GetBeatAnalyzer() { return beatAnalyzer_; }

    TYEngine::AudioSystem::AudioAnalyzer& GetAnalyzer() { return audioAnalyzer_; }

    float& GetWaveform() { return GetAudio()->GetAnalyzerXAPO()->GetWaveform()[0]; }

    void SetHPPerf(float perf) { hpPerf_ = perf; }

private:
    /// <summary>ビートアナライザー（演出用）。</summary>
    TYEngine::AudioSystem::BeatAnalyzer beatAnalyzer_;

    /// <summary>オーディオスペクトラムアナライザー（演出用）。</summary>
    TYEngine::AudioSystem::AudioAnalyzer audioAnalyzer_;

    /// <summary>HP低下演出の制御クラス。</summary>
    TYEngine::CriticalHealthAudioEffect criticalHealthEffect_;

    /// <summary>HPの割合（演出用パラメータ）</summary>
    float hpPerf_ = 1.0f;
};
