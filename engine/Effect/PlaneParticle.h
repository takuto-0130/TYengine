#pragma once
#include "IParticleRenderer.h"

class PlaneParticle : public IParticleRenderer {
public:
    /*void Initialize(DirectXBasis* dx, SrvManager* srv, Camera* cam) override;
    void Update() override;
    void Draw() override;*/

private:

    void CreateResources() override;
};
