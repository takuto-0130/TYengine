#pragma once
#include "AudioSystemBase.h"
#include "SingletonObject.h"

enum class SoundCategory
{
    BGM,
    SE,
    UI,
    CategoryNum
};

class GameAudio :
    public AudioSystemBase<SoundCategory>,
    public SingletonObject<GameAudio>
{
    friend class SingletonObject<GameAudio>;
    friend struct std::default_delete<GameAudio>;

private:
    // 外部からの new/delete を禁止
    GameAudio() = default;
    ~GameAudio() = default;
public:
    void InitCategory()
    {
        CreateCategory(SoundCategory::BGM, "BGM");
        CreateCategory(SoundCategory::SE, "SE");
        CreateCategory(SoundCategory::UI, "UI");
    }
};