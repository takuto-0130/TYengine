#pragma once
#include "IPostEffect.h"
class GaussianEffect :
    public IPostEffect
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input, RenderTexture* output) override;

private:
    struct GaussianParam 
    {
        int kernelSize = 3;
        float sigma = 5.0f;
    };
    std::shared_ptr<GaussianParam> param_;
};

