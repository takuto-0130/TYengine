#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

class ContrailBehaviour : public IParticleBehaviour
{
public:
    void Update(ParticleParam& p, float dt) override;
};
