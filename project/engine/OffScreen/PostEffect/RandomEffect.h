#pragma once
#include "PostEffectBase.h"
class RandomEffect :
    public PostEffectBase
{
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

private:
    struct RandomParam {
        float t = 1.0f;
        float noiseStrength = 0.5f;
        float padding[2];
        Vector3 noiseTint = { 0.3f, 0.3f, 0.3f };
    };
    std::shared_ptr<RandomParam> param_;
};

