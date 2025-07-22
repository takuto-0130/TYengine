#include "Ease.h"
#include <algorithm>

float Ease::Linear(float t)
{
    return t;
}


float Ease::EaseIn(float t, float n)
{
    return powf(t, n);
}
float Ease::EaseOut(float t, float n)
{
    return 1.0f - powf(1.0f - t, n);
}
float Ease::EaseInOut(float t, float n)
{
    return t < 0.5f ? powf(2.0f, n - 1.0f) * powf(t, 2.0f) : 1.0f - powf(-2.0f * t + 2.0f, n) / 2.0f;
}



float Ease::EaseInBack(float t)
{
    float c1 = 1.70158f;
    float c3 = c1 + 1.0f;

    return c3 * powf(t, 3.0f) - c1 * powf(t, 2.0f);
}
float Ease::EaseOutBack(float t)
{
    float c1 = 1.70158f;
    float c3 = c1 + 1.0f;

    return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
}
float Ease::EaseInOutBack(float t)
{
    float c1 = 1.70158f;
    float c2 = c1 * 1.525f;

    return t < 0.5f
        ? (powf(2.0f * t, 2.0f) * ((c2 + 1.0f) * 2.0f * t - c2)) / 2.0f
        : (powf(2.0f * t - 2.0f, 2.0f) * ((c2 + 1.0f) * (t * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;
}



float Ease::EaseInElastic(float t)
{
    if (t == 0.0f || t == 1.0f) return t;
    float c4 = (2.0f * 3.14159265f) / 3.0f;
    return -powf(2.0f, 10.0f * t - 10.0f) * sinf((t * 10.0f - 10.75f) * c4);
}
float Ease::EaseOutElastic(float t)
{
    if (t == 0.0f || t == 1.0f) return t;
    float c4 = (2.0f * 3.14159265f) / 3.0f;
    return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
}
float Ease::EaseInOutElastic(float t)
{
    if (t == 0.0f || t == 1.0f) return t;
    float c5 = (2.0f * 3.14159265f) / 4.5f;
    if (t < 0.5f) {
        return -(powf(2.0f, 20.0f * t - 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f;
    }
    else {
        return (powf(2.0f, -20.0f * t + 10.0f) * sinf((20.0f * t - 11.125f) * c5)) / 2.0f + 1.0f;
    }
}




static float BounceOut(float t)
{
    const float n1 = 7.5625f;
    const float d1 = 2.75f;

    if (t < 1.0f / d1) {
        return n1 * t * t;
    }
    else if (t < 2.0f / d1) {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    }
    else if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    }
    else {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

float Ease::EaseInBounce(float t)
{
    return BounceOut(t);
}
float Ease::EaseOutBounce(float t)
{
    return 1.0f - BounceOut(1.0f - t);
}
float Ease::EaseInOutBounce(float t)
{
    return t < 0.5f
        ? (1.0f - BounceOut(1.0f - 2.0f * t)) * 0.5f
        : (1.0f + BounceOut(2.0f * t - 1.0f)) * 0.5f;
}
