#include "GrayscaleEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void GrayscaleEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Grayscale.PS.hlsl");
    param_ = copyPass_.AddExtraConstantBuffer<GrayscaleParam>(4);
}

void GrayscaleEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Grayscale");
    ImGui::SliderFloat("Strength", &param_->strength, 0.0f, 1.0f);
    ImGui::ColorEdit3("tintColor", &param_->tintColor.x);
    ImGui::End();
#endif // _DEBUG
}

void GrayscaleEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
