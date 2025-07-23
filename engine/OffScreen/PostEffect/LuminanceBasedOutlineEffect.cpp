#include "LuminanceBasedOutlineEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "imgui.h"

void LuminanceBasedOutlineEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/LuminanceBasedOutline.PS.hlsl");
}

void LuminanceBasedOutlineEffect::Update()
{
    copyPass_.Update();
}

void LuminanceBasedOutlineEffect::Apply(RenderTexture* input, RenderTexture* output)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
