#pragma once
#include "ParticleParam.h"

class IParticleBehaviour
{
public:
    virtual ~IParticleBehaviour() = default;
    virtual void Update(ParticleParam& p, float dt) = 0;
};
