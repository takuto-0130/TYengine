#include "ParticleManager.h"

int ParticleManager::Add(std::unique_ptr<IParticleRenderer> particle) {
    particles_.emplace_back(std::move(particle));
    return static_cast<int>(particles_.size()) - int(1);
}

void ParticleManager::InitializeAll(DirectXBasis* dx, SrvManager* srv, Camera* cam) {
    for (auto& p : particles_) {
        p->Initialize(dx, srv, cam);
    }
}

void ParticleManager::UpdateAll() {
    for (auto& p : particles_) {
        p->Update();
    }
}

void ParticleManager::DrawAll() {
    for (auto& p : particles_) {
        p->Draw();
    }
}

void ParticleManager::SetEmitter(int index, IParticleRenderer::ParticleEmitter& emitter)
{
    particles_[index]->SetEmitter(emitter);
}

void ParticleManager::TriggerEmit(int index, bool flag)
{
    if (flag)
    {
        particles_[index]->TriggerEmit();
    }
}
