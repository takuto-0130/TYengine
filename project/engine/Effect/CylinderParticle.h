#pragma once
#include "IParticleRenderer.h"

class CylinderParticle : public IParticleRenderer {
protected:
    void CreateResources() override;
    IParticleRenderer::ParticleP MakeNewParticle(std::mt19937& random, const ParticleEmitter& emitter);
};