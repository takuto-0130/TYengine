#include "GrayscaleEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "imgui.h"

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
    ImGui::SliderFloat("Grayscale Strength", &param_->strength, 0.0f, 1.0f);
    ImGui::End();
#endif // _DEBUG

    copyPass_.Update();
}

void GrayscaleEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
