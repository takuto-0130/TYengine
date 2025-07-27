#pragma once
#include "PostEffectBase.h"

class VignetteEffect
    : public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

private:
    struct VignetteParam
    {
        float intensity = 16.0f;
        float power = 0.8f;
        float padding[2];
        Vector3 vignetteColor = { 0.0f, 0.0f, 0.0f };
    };
    std::shared_ptr<VignetteParam> param_;
};

