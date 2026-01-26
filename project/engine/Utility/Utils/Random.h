#pragma once
#include <random>
#include "SingletonObject.h"

class Random :
    public SingletonObject<Random>
{
    friend class SingletonObject<Random>;
    friend struct std::default_delete<Random>;

private:
    // 外部からの new/delete を禁止
    Random()
    {
        std::random_device rd;
        mt_.seed(rd());
    }
    ~Random() = default;

public:
    // 整数乱数
    int Int(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(mt_);
    }

    // 整数 0 ~ 9
    int Int09()
    {
        std::uniform_int_distribution<int> dist(0, 9);
        return dist(mt_);
    }

    // 浮動小数乱数
    float Float(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(mt_);
    }

    // 0.0f ～ 1.0f
    float Float01()
    {
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        return dist(mt_);
    }

private:
    std::mt19937 mt_;
};
