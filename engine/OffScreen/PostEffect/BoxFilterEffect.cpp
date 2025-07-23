#include "BoxFilterEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "imgui.h"

void BoxFilterEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/BoxFilter.PS.hlsl");
}

void BoxFilterEffect::Update()
{
    copyPass_.Update();
}

void BoxFilterEffect::Apply(RenderTexture* input, RenderTexture* output)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
