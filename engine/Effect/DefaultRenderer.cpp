#include "DefaultRenderer.h"
#include <iostream>

void DefaultRenderer::Draw(const std::list<Particle>& particles, ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso) {
    cmdList->SetGraphicsRootSignature(rootSig);
    cmdList->SetPipelineState(pso);

    for (const auto& p : particles) {
        std::cout << "Draw Particle at: "
            << p.transform.translate.x << ", "
            << p.transform.translate.y << ", "
            << p.transform.translate.z << "\n";
    }
}
