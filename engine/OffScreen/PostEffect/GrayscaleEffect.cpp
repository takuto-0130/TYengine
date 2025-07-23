#include "GrayscaleEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "CopyPass.h"

void GrayscaleEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    copyPass_.Initialize(dx, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/CopyImage.PS.hlsl");
}

void GrayscaleEffect::Update()
{
    // GUIがあればパラメータ反映
    copyPass_.Update();
}

void GrayscaleEffect::Apply(RenderTexture* input, RenderTexture* output)
{
    output->BeginRender();
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
    output->EndRender();
}
