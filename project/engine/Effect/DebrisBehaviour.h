#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

class DebrisBehaviour : public IParticleBehaviour
{
public:
    void Update(ParticleParam& p, float dt) override;
};
