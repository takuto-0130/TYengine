#include "LuminanceBasedOutlineEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void LuminanceBasedOutlineEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/LuminanceBasedOutline.PS.hlsl");
    param_ = copyPass_.AddExtraConstantBuffer<LuminanceOutlineParam>(4);
}

void LuminanceBasedOutlineEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("LuminanceBasedOutline");
    ImGuiUpdate();
    ImGui::End();
#endif // _DEBUG
}

void LuminanceBasedOutlineEffect::ImGuiUpdate()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("LuminanceBasedOutline"))
    {
        ImGui::SliderFloat("Threshold", &param_->threshold, 0.0f, 1.0f);
        ImGui::SliderFloat("EdgeWidth", &param_->edgeWidth, 0.5f, 3.0f);
        ImGui::SliderFloat("Intensity", &param_->edgeIntensity, 0.0f, 2.0f);
        ImGui::ColorEdit3("EdgeColor", &param_->edgeColor.x);
        ImGui::TreePop();
    }
#endif // _DEBUG
}

void LuminanceBasedOutlineEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
