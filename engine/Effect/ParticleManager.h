#pragma once
#include "IParticleRenderer.h"
#include <vector>
#include <memory>



class ParticleManager {
public:
    void Add(std::unique_ptr<IParticleRenderer> particle);
    void InitializeAll(DirectXBasis* dx, SrvManager* srv, Camera* cam);
    void UpdateAll();
    void DrawAll();

private:
    std::vector<std::unique_ptr<IParticleRenderer>> particles_;
};
