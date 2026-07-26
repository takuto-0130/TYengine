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
    /// <summary>
    /// 音声を再生し、BGMカテゴリの場合はアナライザーへ再生ハンドルを登録する。
    /// </summary>
    /// <param name="filename">再生する音声ファイル名。</param>
    /// <param name="isLoop">ループ再生するかどうか。</param>
    /// <param name="category">サウンドカテゴリ（BGM, SE, UI）。</param>
    /// <returns>再生リソースハンドル。</returns>
    int Play(const std::string& filename, bool isLoop, SoundCategory category)
    {
        int handle = TYEngine::AudioSystem::AudioSystemBase<SoundCategory>::Play(filename, isLoop, category);
        if (category == SoundCategory::BGM)
        {
            audioAnalyzer_.SetPlayHandle(handle);
        }
        return handle;
    }

    /// <summary>オーディオ解析およびエフェクトの毎フレーム更新処理。</summary>
    void Update();

    /// <summary>
    /// ビートアナライザー（BPM・リズム解析モジュール）の初期化を行う。
    /// </summary>
    /// <param name="filename">解析対象の音声ファイル名。</param>
    /// <param name="soundCategory">サウンドカテゴリ。</param>
    void InitBeatAnalyzer(const std::string& filename, SoundCategory soundCategory = SoundCategory::BGM);

    /// <summary>指定されたハンドルの音声再生を停止する。</summary>
    /// <param name="resourceNum">再生リソースハンドル。</param>
    void Stop(int resourceNum)
    {
        GetAudio()->Stop(resourceNum);
    }
    /// <summary>指定されたハンドルの音声再生を一時停止する。</summary>
    /// <param name="resourceNum">再生リソースハンドル。</param>
    void Pause(int resourceNum)
    {
        GetAudio()->Pause(resourceNum);
    }
    /// <summary>一時停止中の音声を再開する。</summary>
    /// <param name="resourceNum">再生リソースハンドル。</param>
    void Resume(int resourceNum)
    {
        GetAudio()->Resume(resourceNum);
    }

    /// <summary>音波のピッチ（周波数倍率）を変更する。</summary>
    /// <param name="resourceNum">再生リソースハンドル。</param>
    /// <param name="pitch">ピッチ値（1.0fが標準）。</param>
    void Pitch(int resourceNum, float pitch)
    {
        GetAudio()->Pitch(resourceNum, pitch);
    }

    /// <summary>ビート解析モジュールを取得する。</summary>
    /// <returns>BeatAnalyzer 参照。</returns>
    TYEngine::AudioSystem::BeatAnalyzer& GetBeatAnalyzer() { return beatAnalyzer_; }

    /// <summary>オーディオ解析モジュールを取得する。</summary>
    /// <returns>AudioAnalyzer 参照。</returns>
    TYEngine::AudioSystem::AudioAnalyzer& GetAnalyzer() { return audioAnalyzer_; }

    /// <summary>波形データへの参照を取得する。</summary>
    /// <returns>波形サンプルデータ値参照。</returns>
    float& GetWaveform() { return const_cast<float&>(audioAnalyzer_.GetWaveform()[0]); }

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
