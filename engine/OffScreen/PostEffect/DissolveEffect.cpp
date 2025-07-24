#include "DissolveEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void DissolveEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Dissolve.PS.hlsl");
    copyPass_.LoadAndSetMaskTexture("Resources/noise0.png");
    param_ = copyPass_.AddExtraConstantBuffer<DissolveParam>(4);
}

void DissolveEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Dissolve");
    ImGui::SliderFloat("threshold", &param_->threshold, 0.0f, 1.0f);
    bool useEdge = (param_->useEdge != 0);
    if (ImGui::Checkbox("useEdge", &useEdge)) {
        param_->useEdge = useEdge ? 1 : 0;
    }
    ImGui::ColorEdit3("edgeColor", &param_->edgeColor.x);
    ImGui::End();
#endif // _DEBUG
}

void DissolveEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
