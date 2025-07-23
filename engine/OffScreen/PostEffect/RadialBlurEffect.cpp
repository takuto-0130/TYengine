#include "RadialBlurEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "imgui.h"

void RadialBlurEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/RadialBlur.PS.hlsl");
    param_ = copyPass_.AddExtraConstantBuffer<RadialBlurParam>(4);
}

void RadialBlurEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("RadialBlur");
    ImGui::SliderFloat2("Center", &param_->kCenter.x, 0.0f, 1.0f);
    ImGui::SliderFloat("BlurWidth", &param_->kBlurWidth, 0.0f, 1.0f);
    ImGui::SliderInt("Sample", &param_->kNumSamples, 1, 50);
    ImGui::End();
#endif // _DEBUG
    copyPass_.Update();
}

void RadialBlurEffect::Apply(RenderTexture* input, RenderTexture* output)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
