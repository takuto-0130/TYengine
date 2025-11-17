#pragma once
#include <Windows.h>

// 時間管理クラス
class Timer {
public: // メンバ関数
    // シングルトンインスタンス
    static Timer* GetInstance()
    {
        static Timer instance;
        return &instance;
    }

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

    // コピー・ムーブ禁止
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

private: // メンバ関数
    // コンストラクタ
    Timer() = default;
    // デストラクタ
    ~Timer() = default;

private: // メンバ変数
    LARGE_INTEGER frequency_{};
    LARGE_INTEGER previousTime_{};

    float deltaTime_ = 0.0f;
    float timeScale_ = 1.0f;
    float timeAccumulator_ = 0.0f;

    int frameCount_ = 0;
    int fps_ = 0;
};
