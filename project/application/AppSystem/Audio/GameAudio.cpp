#include "GameAudio.h"
#include "Timer.h"
#include "Utils/Json/JsonManager.h"

// GameAudio クラスの実装
void GameAudio::OnInit()
{
    CreateCategory(SoundCategory::BGM, "BGM");
    CreateCategory(SoundCategory::SE, "SE");
    CreateCategory(SoundCategory::UI, "UI");
    InitBeatAnalyzer("gameBGM");

    // Config.json からレイテンシ補正値（AudioLatencyOffset）をロードして適用
    TYEngine::Utility::JsonManager jm;
    std::string err;
    if (jm.Load("Config.json", true, &err))
    {
        // もしキーが存在しなければ初期値（0.08f）をセットして保存
        if (!jm.Root().contains("AudioLatencyOffset"))
        {
            jm.Set("AudioLatencyOffset", 0.08f);
            jm.Save();
        }
        float offset = jm.Get<float>("AudioLatencyOffset", 0.08f);
        audioAnalyzer_.SetLatencyOffset(offset);
    }
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
