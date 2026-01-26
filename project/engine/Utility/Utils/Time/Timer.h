#pragma once
#include <Windows.h>
#include "SingletonObject.h"

// 時間管理クラス
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
    // タイマー開始（初期化時に1度だけ呼ぶ）
    void Start();
    // 更新
    void Update();

    // デルタタイム取得（タイムスケール適用後）
    float GetDeltaTime() const { return deltaTime_ * timeScale_; }
    // デルタタイム取得（タイムスケール適用前）
    float GetRawDeltaTime() const { return deltaTime_; }
    // フレームレート取得（整数値）
    int GetFPS() const { return fps_; }

    // setter / getter
    void SetTimeScale(float scale) { timeScale_ = scale; }
    float GetTimeScale() const { return timeScale_; }

private: // メンバ変数
    LARGE_INTEGER frequency_{};
    LARGE_INTEGER previousTime_{};

    float deltaTime_ = 0.0f;
    float timeScale_ = 1.0f;
    float timeAccumulator_ = 0.0f;

    int frameCount_ = 0;
    int fps_ = 0;
};
