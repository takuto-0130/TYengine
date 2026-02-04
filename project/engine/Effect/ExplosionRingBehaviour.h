#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

namespace TYEngine {
namespace Effect {

class ExplosionRingBehaviour : public IParticleBehaviour
{
public:
    void Update(ParticleParam& p, float dt) override;
};

} // namespace Effect
} // namespace TYEngine
