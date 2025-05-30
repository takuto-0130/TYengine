#include "ParticleManager.h"

void ParticleManager::Add(std::unique_ptr<IParticleRenderer> particle) {
    particles_.emplace_back(std::move(particle));
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
