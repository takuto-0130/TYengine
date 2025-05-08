#pragma once
#include "IEmitter.h"
#include "IUpdater.h"
#include "IRenderer.h"
#include <memory>

class ParticleSystem {
public:
    void SetEmitter(std::unique_ptr<IEmitter> emitter);
    void AddUpdater(std::unique_ptr<IUpdater> updater);
    void SetRenderer(std::unique_ptr<IRenderer> renderer);

    void Update(float deltaTime);
    void Draw(ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso);

private:
    std::list<Particle> particles_;
    std::unique_ptr<IEmitter> emitter_;
    std::vector<std::unique_ptr<IUpdater>> updaters_;
    std::unique_ptr<IRenderer> renderer_;
};
