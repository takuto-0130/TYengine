#pragma once
#include "IParticleRenderer.h"

class CylinderParticle : public IParticleRenderer {
protected:
    void CreateResources() override;
};