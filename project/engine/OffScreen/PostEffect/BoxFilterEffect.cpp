#include "BoxFilterEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void BoxFilterEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    // シェーダ（CopyImage.VSとBoxFilter.PS）を使う
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/BoxFilter.PS.hlsl");
    // パラメータバッファ確保
    param_ = copyPass_.AddExtraConstantBuffer<BoxFilterParam>(4);
}

void BoxFilterEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("BoxFilter");
    ImGuiUpdate();
    ImGui::End();
#endif // _DEBUG
}

void BoxFilterEffect::ImGuiUpdate()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("BoxFilter"))
    {
        ImGui::SliderInt("kernelSize", &param_->kernelSize, 1, 15);
        ImGui::TreePop();
    }
#endif // _DEBUG
}

void BoxFilterEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
