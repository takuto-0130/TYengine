#include "CopyPass.h"
#include "SrvManager.h"
#include "TextureManager.h"
#include "imgui.h"
#include <cassert>

namespace TYEngine {
namespace OffScreen {

using namespace Core; // For DirectXBasis
using namespace Graphics; // For TextureManager (Likely in TYEngine::Graphics if moved, otherwise global or standard) 
// Wait, TextureManager header might be in graphics? No, found it previously?
// Assuming TextureManager is global or imported. The include is "TextureManager.h".
// If TextureManager is not yet refactored, it's global.
// I'll add 'using namespace Graphics;' IF TextureManager is there. But I haven't refactored TextureManager yet.
// So I shouldn't add 'using namespace Graphics;' yet unless it breaks.
// SAFE: Global TextureManager.

void CopyPass::Initialize(DirectXBasis* dxBasis, SrvManager* srvMgr, const std::wstring& vsPath, const std::wstring& psPath) {
    dxBasis_ = dxBasis;
    srvMgr_ = srvMgr;

    copyParamBuffer_ = dxBasis_->CreateBufferResource(sizeof(CopyPassParam));

    // Map して書き込み先ポインタを取得
    CD3DX12_RANGE readRange(0, 0);
    copyParamBuffer_->Map(0, &readRange, reinterpret_cast<void**>(&mappedParam_));

    CD3DX12_DESCRIPTOR_RANGE range[2]{};
    range[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    range[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    CD3DX12_DESCRIPTOR_RANGE samplerRange{};
    samplerRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

    CD3DX12_ROOT_PARAMETER rootParams[6]{};
    rootParams[0].InitAsDescriptorTable(1, &range[0], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParams[3].InitAsDescriptorTable(1, &range[1], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParams[1].InitAsDescriptorTable(1, &samplerRange, D3D12_SHADER_VISIBILITY_PIXEL);
    rootParams[2].InitAsConstantBufferView(0); // b0: CopyPassParam
    rootParams[4].InitAsConstantBufferView(1); // b1: ExtraEffectParamA
    rootParams[5].InitAsConstantBufferView(2); // b2: ExtraEffectParamB

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc{};
    rootSigDesc.Init(_countof(rootParams), rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
    dxBasis_->GetDevice()->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));

    auto vs = dxBasis_->CompileShader(vsPath, L"vs_6_0");
    auto ps = dxBasis_->CompileShader(psPath, L"ps_6_0");

    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
    psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
    psoDesc.pRootSignature = rootSignature_.Get();
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    psoDesc.NumRenderTargets = 1;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.InputLayout = { nullptr, 0 };
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.DSVFormat = DXGI_FORMAT_UNKNOWN; // ← ここが重要
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    dxBasis_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));

    // --- Create Sampler Heap in DirectXBasis ---
    dxBasis_->CreateSamplerHeap();
    mappedParam_->offset = { 0.0f, 0.0f }; // 中央
    mappedParam_->scale = { 1.0f, 1.0f }; // フルサイズ
}

void CopyPass::Update()
{
#ifdef _DEBUG
    ImGui::Begin("render");
    ImGui::DragFloat2("offset", &mappedParam_->offset.x, 0.01f);
    ImGui::DragFloat2("scale", &mappedParam_->scale.x, 0.01f);
    ImGui::End();
#endif // _DEBUG

}

void CopyPass::Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, bool toSwapChain) {
    cmdList->SetPipelineState(pipelineState_.Get());
    cmdList->SetGraphicsRootSignature(rootSignature_.Get());
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ID3D12DescriptorHeap* heaps[] = {
        srvMgr_->GetHeap(),
        dxBasis_->GetSamplerHeap()
    };
    cmdList->SetDescriptorHeaps(_countof(heaps), heaps);

    // テクスチャセット
    cmdList->SetGraphicsRootDescriptorTable(0, srvHandle);
    if (useExtraTexture_)
    {
        cmdList->SetGraphicsRootDescriptorTable(3, extraSrvHandle_);
    }
    cmdList->SetGraphicsRootDescriptorTable(1, dxBasis_->GetSamplerDescriptorHandle());
    cmdList->SetGraphicsRootConstantBufferView(2, copyParamBuffer_->GetGPUVirtualAddress());

    // 追加バッファセット
    for (const auto& extra : extraBuffers_) {
        cmdList->SetGraphicsRootConstantBufferView(extra.registerIndex, extra.resource->GetGPUVirtualAddress());
    }

    // スワップチェーンへの描画ならビューポート設定
    if (toSwapChain) {
        D3D12_VIEWPORT vp = dxBasis_->GetViewport();
        D3D12_RECT scissor = dxBasis_->GetScissorRect();
        cmdList->RSSetViewports(1, &vp);
        cmdList->RSSetScissorRects(1, &scissor);
    }

    // 画面全体への描画（大きな三角形1枚など）
    cmdList->DrawInstanced(3, 1, 0, 0);
}

void CopyPass::LoadAndSetMaskTexture(const std::string& filePath)
{
    TextureManager* texMgr = TextureManager::GetInstance();
    texMgr->LoadTexture(filePath);
    extraSrvHandle_ = texMgr->GetSrvHandleGPU(filePath);
    useExtraTexture_ = true;
}

} // namespace OffScreen
} // namespace TYEngine
