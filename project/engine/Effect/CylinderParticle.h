#pragma once
#include "IParticleRenderer.h"
#include "ParticleParam.h"

/// <summary>
/// 円筒（シリンダー）形状のパーティクルレンダラー。
/// ノズル噴射やビームエフェクトなどに使用される。
/// </summary>
class CylinderParticle : public IParticleRenderer {
protected:
    void CreateResources() override;
    ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter);
};