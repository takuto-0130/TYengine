#include "GameAudio.h"
#include "Timer.h"

// GameAudio クラスの実装
void GameAudio::OnInit()
{
    CreateCategory(SoundCategory::BGM, "BGM");
    CreateCategory(SoundCategory::SE, "SE");
    CreateCategory(SoundCategory::UI, "UI");
    InitBeatAnalyzer("gameBGM");
}

void GameAudio::Update()
{
    beatAnalyzer_.Update();
    audioAnalyzer_.Update();
    audioAnalyzer_.Draw();

    // HP低下時のオーディオ・ポストエフェクト連携演出を更新
    float deltaTime = TYEngine::Utility::Timer::GetInstance()->GetDeltaTime();
    criticalHealthEffect_.Update(deltaTime, hpPerf_, GetAudio()->GetAnalyzerXAPO());
}

void GameAudio::InitBeatAnalyzer(const std::string& filename, SoundCategory soundCategory)
{
    beatAnalyzer_.Init(filename, CategoryToString(soundCategory));
}
