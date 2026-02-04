#include "OutlinePass.h"
#include "DirectXBasis.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

namespace TYEngine {
namespace OffScreen {

using namespace Core; // For DirectXBasis

void OutlinePass::Initialize(DirectXBasis* dx, SrvManager* srvMgr) {
    // シェーダ（CopyImage.VSとDepthBasedOutline.PS）を使う
    copyPass_.Initialize(dx, srvMgr, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/DepthBasedOutline.PS.hlsl");
    // アウトライン用パラメータバッファ確保
    outlineParam_ = copyPass_.AddExtraConstantBuffer<OutlinePassParam>(4);
}

void OutlinePass::SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle) {
    copyPass_.SetDepthSrv(handle); // register(t1) にバインド
}

void OutlinePass::ImGuiUpdate()
{
#ifdef _DEBUG
    ImGui::ColorEdit3("color", &outlineParam_->outlineColor.x);
    ImGui::SliderFloat("depthThreshold", &outlineParam_->depthThreshold, 0.001f, 0.008f, "%.5f");
#endif // _DEBUG
}

void OutlinePass::Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv) {
    copyPass_.Draw(cmdList, inputSrv);
}

void OutlinePass::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Outline");
    ImGuiUpdate();
    ImGui::End();
#endif // _DEBUG
}

} // namespace OffScreen
} // namespace TYEngine
