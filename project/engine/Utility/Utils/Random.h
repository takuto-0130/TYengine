#pragma once
#include <random>

class Random
{
public:
    static Random* GetInstance()
    {
        static Random instance;
        return &instance;
    }

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
    Random()
    {
        std::random_device rd;
        mt_.seed(rd());
    }

    ~Random() = default;

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

private:
    std::mt19937 mt_;
};
