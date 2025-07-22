#pragma once
#include <Windows.h>

class Timer {
public:
    static Timer* GetInstance()
    {
        static Timer instance;
        return &instance;
    }

    void Start();

    void Update();

    float GetDeltaTime() const { return deltaTime_ * timeScale_; }
    float GetRawDeltaTime() const { return deltaTime_; } // スケール前
    int GetFPS() const { return fps_; }

    void SetTimeScale(float scale) { timeScale_ = scale; }
    float GetTimeScale() const { return timeScale_; }

private:
    Timer() = default;
    ~Timer() = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

private:
    LARGE_INTEGER frequency_{};
    LARGE_INTEGER previousTime_{};
    float deltaTime_ = 0.0f;
    float timeScale_ = 1.0f;

    int frameCount_ = 0;
    float timeAccumulator_ = 0.0f;
    int fps_ = 0;
};
