#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

class ExplosionRingBehaviour : public IParticleBehaviour
{
public:
    void Update(ParticleParam& p, float dt) override;
};
