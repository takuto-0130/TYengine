#pragma once
#include "IParticleRenderer.h"

class RingParticle : public IParticleRenderer {
protected:
    void CreateResources() override;
};

