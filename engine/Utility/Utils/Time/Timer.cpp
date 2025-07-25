#include "Timer.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


void Timer::Start()
{
    QueryPerformanceFrequency(&frequency_);
    QueryPerformanceCounter(&previousTime_);
    frameCount_ = 0;
    fps_ = 0;
    timeAccumulator_ = 0.0f;
}

void Timer::Update()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    deltaTime_ = static_cast<float>(currentTime.QuadPart - previousTime_.QuadPart)
        / static_cast<float>(frequency_.QuadPart);

    previousTime_ = currentTime;

    // FPS 計測
    ++frameCount_;
    timeAccumulator_ += deltaTime_;

    if (timeAccumulator_ >= 1.0f) {
        fps_ = frameCount_;
        frameCount_ = 0;
        timeAccumulator_ -= 1.0f;
    }

#ifdef _DEBUG
    ImGui::Begin("Timer");
    ImGui::Text("DeltaTime : %.5f", GetDeltaTime());
    ImGui::Text("RawDeltaTime : %.5f", GetRawDeltaTime());
    ImGui::Text("fps : %d", fps_);
    ImGui::SliderFloat("TimeScale", &timeScale_, 0.01f, 1.0f);
    ImGui::End();
#endif // _DEBUG

}
