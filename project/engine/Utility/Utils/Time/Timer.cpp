#include "Timer.h"
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


namespace TYEngine
{
	namespace Utility
	{


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

			// ゲーム内時間の更新（TimeScale反映なし版を使用したい場合はGetRawDeltaTimeを使用）
			// deltaTime_ は生の経過時間。TimeScaleの反映は取得側で調整するか、ここで行うかは設計方針によるが、
			// ここではRawDeltaTimeとして保持し、TimeScaleはGetter等で反映させることが多いが、
			// この実装では deltaTime_ は Raw 値。
			// (BulletTimeController等でTimeScale管理している)

			// FPS 計測
			++frameCount_;
			timeAccumulator_ += deltaTime_;
			if (timeAccumulator_ >= 1.0f)
			{
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

	} // namespace Utility
} // namespace TYEngine
