#include "GaussianEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void GaussianEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Gaussian.PS.hlsl");
    param_ = copyPass_.AddExtraConstantBuffer<GaussianParam>(4);
}

void GaussianEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Gaussian");
    ImGuiUpdate();
    ImGui::End();
#endif // _DEBUG
}

void GaussianEffect::ImGuiUpdate()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("Gaussian"))
    {
        ImGui::SliderInt("KernelSize", &param_->kernelSize, 1, 15); // 最大値はPSに合わせる
        ImGui::SliderFloat("Sigma", &param_->sigma, 0.1f, 7.0f); // kernelSizeを15にしても7移行は変化量が小さすぎるので最大値は7

        if (ImGui::TreeNode("Help"))
        {
            ImGui::Text("Size 3   : Sigma 0.3~0.8");
            ImGui::Text("Size 5   : Sigma 0.5~1.5");
            ImGui::Text("Size 7   : Sigma 1.0~2.5");
            ImGui::Text("Size 9   : Sigma 1.5~3.5");
            ImGui::Text("Size 11+ : Sigma 2.5~5.0+");
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
#endif // _DEBUG
}

void GaussianEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
