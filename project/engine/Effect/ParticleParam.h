#pragma once
#include "struct.h"

struct ParticleParam
{
    Transform transform;
    Vector3 velocity;
    Vector4 color;
    float lifeTime = 0.0f;
    float currentTime = 0.0f;
};
