#include "ParticleSystem.h"
#include <random>

void ParticleSystem::SetEmitter(std::unique_ptr<IEmitter> emitter) {
    emitter_ = std::move(emitter);
}

void ParticleSystem::AddUpdater(std::unique_ptr<IUpdater> updater) {
    updaters_.push_back(std::move(updater));
}

void ParticleSystem::SetRenderer(std::unique_ptr<IRenderer> renderer) {
    renderer_ = std::move(renderer);
}

void ParticleSystem::Update(float deltaTime) {
    static std::random_device rd;
    static std::mt19937 rng(rd());

    if (emitter_) {
        auto newParticles = emitter_->Emit(rng);
        particles_.splice(particles_.end(), newParticles);
    }

    for (auto& updater : updaters_) {
        updater->Update(particles_, deltaTime);
    }
}

void ParticleSystem::Draw(ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso) {
    if (renderer_) {
        renderer_->Draw(particles_, cmdList, rootSig, pso);
    }
}
