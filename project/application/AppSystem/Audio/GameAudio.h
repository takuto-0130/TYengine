#pragma once
#include "AudioSystemBase.h"
#include "SingletonObject.h"

enum class SoundCategory
{
    BGM,
    SE,
    UI
};

class GameAudio :
    public AudioSystemBase<SoundCategory>,
    public SingletonObject<GameAudio>
{
    friend class SingletonObject<GameAudio>;
    friend struct std::default_delete<GameAudio>;

private:
    // 外部からの new/delete を禁止
    GameAudio()
    {
        // 初期のカテゴリー登録
        CreateCategory(SoundCategory::BGM, "BGM");
        CreateCategory(SoundCategory::SE, "SE");
        CreateCategory(SoundCategory::UI, "UI");
    }
    ~GameAudio() = default;
};