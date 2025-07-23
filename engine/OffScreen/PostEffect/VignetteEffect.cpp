#include "VignetteEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "imgui.h"

void VignetteEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Vignette.PS.hlsl");
}

void VignetteEffect::Update()
{
    copyPass_.Update();
}

void VignetteEffect::Apply(RenderTexture* input, RenderTexture* output)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
