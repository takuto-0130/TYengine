#include "RandomEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

void RandomEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dx_ = dx;
    // シェーダ（CopyImage.VSとRandom.PS）を使う
    copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/Random.PS.hlsl");
    // パラメータバッファ確保
    param_ = copyPass_.AddExtraConstantBuffer<RandomParam>(4);
}

void RandomEffect::Update()
{
#ifdef _DEBUG
    ImGui::Begin("Random");
    ImGuiUpdate();
    ImGui::End();
#endif // _DEBUG
}

void RandomEffect::ImGuiUpdate()
{
#ifdef _DEBUG
    if (ImGui::TreeNode("Random"))
    {
        ImGui::SliderFloat("t", &param_->t, 0.0f, 1.0f);
        ImGui::SliderFloat("NoiseStrength", &param_->noiseStrength, 0.0f, 1.0f);
        ImGui::ColorEdit3("NoiseTint", &param_->noiseTint.x);
        ImGui::TreePop();
    }
#endif // _DEBUG
}

void RandomEffect::Apply(RenderTexture* input)
{
    copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
}
