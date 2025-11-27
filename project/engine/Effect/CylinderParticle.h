#pragma once
#include "IParticleRenderer.h"
#include "ParticleParam.h"

class CylinderParticle : public IParticleRenderer {
protected:
    void CreateResources() override;
    ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter);
};