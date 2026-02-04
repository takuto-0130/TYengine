#include "ParticleManager.h"

namespace TYEngine {
namespace Effect {

using namespace Camera;

int ParticleManager::Add(std::unique_ptr<IParticleRenderer> particle) {
    // パーティクルシステムをリストに追加して管理
    particles_.emplace_back(std::move(particle));
    // インデックスを返す
    return static_cast<int>(particles_.size()) - int(1);
}

void ParticleManager::InitializeAll(DirectXBasis* dx, SrvManager* srv, Camera::Camera* cam) {
    // 全パーティクルシステムの初期化
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

} // namespace Effect
} // namespace TYEngine
