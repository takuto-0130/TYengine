#include "VignetteEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void VignetteEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    // シェーダ（CopyImage.VSとVignette.PS）を使う
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Vignette.PS.hlsl");
    // パラメータバッファ確保
    param_ = copyPass_.AddExtraConstantBuffer<VignetteParam>(4);
}

void VignetteEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Vignette");
    ImGuiUpdate();
    ImGui::End();
#endif // _DEBUG
}

void VignetteEffect::ImGuiUpdate()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("Vignette")) 
    {
        ImGui::SliderFloat("intensity", &param_->intensity, 2.0f, 30.0f);
        ImGui::SliderFloat("power", &param_->power, 0.0f, 3.0f);
        ImGui::ColorEdit3("color", &param_->vignetteColor.x);
        ImGui::TreePop();
    }
#endif // _DEBUG
}

void VignetteEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
