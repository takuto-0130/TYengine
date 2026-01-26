#pragma once
#include "IParticleRenderer.h"
#include "SingletonObject.h"
#include <vector>
#include <memory>



class ParticleManager :
    public SingletonObject<ParticleManager>
{
    friend class SingletonObject<ParticleManager>;
    friend struct std::default_delete<ParticleManager>;

private:
    // 外部からの new/delete を禁止
    ParticleManager() = default;
    ~ParticleManager() = default;

public:
    int Add(std::unique_ptr<IParticleRenderer> particle);
    void InitializeAll(DirectXBasis* dx, SrvManager* srv, Camera* cam);
    void UpdateAll();
    void DrawAll();

public:
    void SetEmitter(int index, IParticleRenderer::Emitter& emitter);

    void TriggerEmit(int index, bool flag);

private:
    std::vector<std::unique_ptr<IParticleRenderer>> particles_;
};
