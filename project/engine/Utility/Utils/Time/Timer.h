#pragma once
#include <Windows.h>
#include "SingletonObject.h"

/// <summary>
/// 時間管理クラス（シングルトン）。
/// デルタタイムの計測、FPS計算、タイムスケール機能を提供する。
/// </summary>
class Timer :
    public SingletonObject<Timer>
{
    friend class SingletonObject<Timer>;
    friend struct std::default_delete<Timer>;

private:
    // 外部からの new/delete を禁止
    Timer() = default;
    ~Timer() = default;

public:
    /// <summary>
    /// タイマーを開始する（初期化時に1度だけ呼ぶ）。
    /// </summary>
    void Start();
    /// <summary>
    /// 毎フレームの更新処理。
    /// デルタタイムやFPSの計算を行う。
    /// </summary>
    void Update();

    /// <summary>
    /// タイムスケール適用後のデルタタイム（秒）を取得する。
    /// ゲームロジックの移動量計算などに使用する。
    /// </summary>
    float GetDeltaTime() const { return deltaTime_ * timeScale_; }

    /// <summary>
    /// 実時間のデルタタイム（秒）を取得する。
    /// UIアニメーションなどタイムスケールの影響を受けたくない場合に使用する。
    /// </summary>
    float GetRawDeltaTime() const { return deltaTime_; }

    /// <summary>
    /// 現在のフレームレート（整数値）を取得する。
    /// </summary>
    int GetFPS() const { return fps_; }

    // setter / getter
    void SetTimeScale(float scale) { timeScale_ = scale; }
    float GetTimeScale() const { return timeScale_; }

private: // メンバ変数
    /// <summary>パフォーマンスカウンタの周波数。</summary>
    LARGE_INTEGER frequency_{};
    /// <summary>前回計測時のカウント値。</summary>
    LARGE_INTEGER previousTime_{};

    /// <summary>前フレームからの経過時間（秒）。</summary>
    float deltaTime_ = 0.0f;
    /// <summary>タイムスケール（スローや早送り用）。</summary>
    float timeScale_ = 1.0f;
    /// <summary>FPS計測用の累積時間。</summary>
    float timeAccumulator_ = 0.0f;

    /// <summary>FPS計測用のフレームカウンタ。</summary>
    int frameCount_ = 0;
    /// <summary>現在のFPS（Frames Per Second）。</summary>
    int fps_ = 0;
};
