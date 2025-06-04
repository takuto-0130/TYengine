#include "ParticleManager.h"

int ParticleManager::Add(std::unique_ptr<IParticleRenderer> particle) {
    particles_.emplace_back(std::move(particle));
    return particles_.size() - (size_t)1;
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

void ParticleManager::SetEmitter(int index, IParticleRenderer::Emitter& emitter)
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
