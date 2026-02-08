#include "BeatAnalyzer.h"
#include "Audio.h"
#include "Timer.h"
#include "imgui.h"


namespace TYEngine
{
	namespace AudioSystem
	{
		BeatAnalyzer::BeatAnalyzer()
		{
		
		}
		void BeatAnalyzer::Init(const std::string& filename, const std::string& soundCategory)
		{
			currentBPM_ = Audio::GetInstance()->GetSoundData(filename).bpm;
			soundCategory_ = soundCategory;
		}
		void BeatAnalyzer::Init(float bpm, const std::string& soundCategory)
		{
			currentBPM_ = bpm;
			soundCategory_ = soundCategory;
		}
		void BeatAnalyzer::Update()
		{
			DetectBeat();
		}
		void BeatAnalyzer::Draw()
		{
#ifdef _DEBUG

			ImGui::Begin("Beat Analyzer");
			ImGui::Text("BPM : %.2f", currentBPM_);
			ImGui::Text("Beat : %d", isBeat_);

			ImGui::End();

#endif // _DEBUG
		}
		void BeatAnalyzer::DetectBeat()
		{
			// 無音判定
			if (Audio::GetInstance()->GetAnalyzerXAPO(soundCategory_)->GetRMS() < 0.00001f)
			{
				isBeat_ = false;
				beatPhase_ = 0.0f;
				return;
			}

			// XAPOからSpectral Flux取得
			float currentFlux = Audio::GetInstance()->GetAnalyzerXAPO(soundCategory_)->GetSpectralFlux();

			// 1. 平均Flux計算
			fluxHistory_.push_back(currentFlux);
			if (fluxHistory_.size() > 60) fluxHistory_.erase(fluxHistory_.begin());

			float avgFlux = 0;
			for (float f : fluxHistory_) avgFlux += f;
			avgFlux /= fluxHistory_.size();

			// 2. 閾値計算
			float threshold = avgFlux * 1.8f;

			// メトロノーム更新
			float deltaTime = TYEngine::Utility::Timer::GetInstance()->GetDeltaTime();

			// ガード
			if (currentBPM_ <= 0.0f) currentBPM_ = 120.0f;

			float beatDuration = 60.0f / currentBPM_;
			beatPhase_ += deltaTime / beatDuration;

			// クールダウン更新
			if (syncCooldown_ > 0.0f) syncCooldown_ -= deltaTime;

			// 同期処理（タイミング合わせのみ）
			if (currentFlux > threshold && syncCooldown_ <= 0.0f)
			{
				float phase = fmod(beatPhase_, 1.0f);

				// 拍の前後（1.0 または 0.0）に近い場合だけ補正
				if (phase > 0.8f || phase < 0.2f)
				{
					// エラー計算 (正:遅れてる、負:進みすぎ)
					float error = (phase > 0.5f) ? (1.0f - phase) : -phase;

					// 位相（タイミング）の補正を行う
					beatPhase_ += error * 0.2f;

					// クールダウン
					syncCooldown_ = 0.2f;
				}
			}

			// 拍通知
			if (beatPhase_ >= 1.0f)
			{
				beatPhase_ -= 1.0f;
				isBeat_ = true;
			}
			else
			{
				isBeat_ = false;
			}
		}
	}
}