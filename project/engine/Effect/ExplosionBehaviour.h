#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

namespace TYEngine {
namespace Effect {

/// <summary>
/// 爆発エフェクト用の挙動クラス。
/// 放射状に拡散し減速する動きなどを定義する。
/// </summary>
class ExplosionBehaviour : public IParticleBehaviour
{
public:
    /// <summary>
    /// パーティクルの更新（爆発挙動の適用）。
    /// </summary>
    void Update(ParticleParam& p, float dt) override;
};

} // namespace Effect
} // namespace TYEngine
