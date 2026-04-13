#include "GameAudio.h"

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
}

void GameAudio::InitBeatAnalyzer(const std::string& filename, SoundCategory soundCategory)
{
    beatAnalyzer_.Init(filename, CategoryToString(soundCategory));
}
