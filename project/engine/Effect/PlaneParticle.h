#pragma once
#include "IParticleRenderer.h"

class PlaneParticle : public IParticleRenderer {
public:
private:
    void CreateResources() override;
    ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter) override;
};
