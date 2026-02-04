#pragma once
#include "IParticleBehaviour.h"
#include "ParticleParam.h"

namespace TYEngine {
namespace Effect {

/// <summary>
/// 破片（デブリ）パーティクル用の挙動クラス。
/// 重力落下や回転など、物理的な破片の動きを定義する。
/// </summary>
class DebrisBehaviour : public IParticleBehaviour
{
public:
    /// <summary>
    /// パーティクルの更新（デブリ挙動の適用）。
    /// </summary>
    void Update(ParticleParam& p, float dt) override;
};

} // namespace Effect
} // namespace TYEngine
