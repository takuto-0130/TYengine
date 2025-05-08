#pragma once
#include "Particle.h"
#include <list>
#include <d3d12.h>

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void Draw(const std::list<Particle>& particles, ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso) = 0;
};
