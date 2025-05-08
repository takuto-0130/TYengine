#pragma once
#include "IRenderer.h"

class DefaultRenderer : public IRenderer {
public:
    void Draw(const std::list<Particle>& particles, ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso) override;
};
