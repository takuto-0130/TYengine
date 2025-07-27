#pragma once
#include "PostEffectBase.h"
class GaussianEffect :
    public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

private:
    struct GaussianParam 
    {
        int kernelSize = 3;
        float sigma = 5.0f;
    };
    std::shared_ptr<GaussianParam> param_;
};

