#pragma once
#include "IParticleRenderer.h"

class PlaneParticle : public IParticleRenderer {
public:
    /*void Initialize(DirectXBasis* dx, SrvManager* srv, Camera* cam) override;
    void Update() override;
    void Draw() override;*/

    void TriggerEmit() override;

private:
    void CreateResources() override;
    ParticleP MakeNewParticle(std::mt19937& random, const ParticleEmitter& emitter) override;
    void Update() override;
    void Draw() override;
};
