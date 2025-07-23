#include "DissolveEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "imgui.h"

void DissolveEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Dissolve.PS.hlsl");
    copyPass_.LoadAndSetMaskTexture("Resources/noise1.png");
    param_ = copyPass_.AddExtraConstantBuffer<DissolveParam>(4);
}

void DissolveEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Dissolve");
    ImGui::SliderFloat("threshold", &param_->threshold, 0.0f, 1.0f);
    ImGui::Checkbox("useEdge", &param_->useEdge);
    ImGui::End();
#endif // _DEBUG

    copyPass_.Update();
}

void DissolveEffect::Apply(RenderTexture* input, RenderTexture* output)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
