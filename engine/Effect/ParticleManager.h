#pragma once
#include "IParticleRenderer.h"
#include <vector>
#include <memory>



class ParticleManager {
public:
    static ParticleManager* GetInstance() {
        static ParticleManager instance;
        return &instance;
    }
    int Add(std::unique_ptr<IParticleRenderer> particle);
    void InitializeAll(DirectXBasis* dx, SrvManager* srv, Camera* cam);
    void UpdateAll();
    void DrawAll();

public:
    void SetEmitter(int index, IParticleRenderer::Emitter& emitter);

private:
    std::vector<std::unique_ptr<IParticleRenderer>> particles_;
};
