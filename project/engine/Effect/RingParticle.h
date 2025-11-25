#pragma once
#include "IParticleRenderer.h"

class RingParticle : public IParticleRenderer {
protected:
    void CreateResources() override;
    ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter) override;
    std::list<ParticleParam> Emit(std::mt19937& random)override;
    //void Update() override;
};

