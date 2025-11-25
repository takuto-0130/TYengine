#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

class ExplosionBehaviour : public IParticleBehaviour
{
public:
    void Update(ParticleParam& p, float dt) override;
};
