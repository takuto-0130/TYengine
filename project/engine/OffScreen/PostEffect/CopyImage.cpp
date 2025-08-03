#include "CopyImage.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"

void CopyImageEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/CopyImage.PS.hlsl");
}

void CopyImageEffect::Update()
{
}

void CopyImageEffect::ImGuiUpdate()
{
}

void CopyImageEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
