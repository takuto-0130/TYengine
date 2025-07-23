#pragma once
#include "IPostEffect.h"

class GrayscaleEffect
	: public IPostEffect
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input, RenderTexture* output) override;

private:
    struct GrayscaleParam {
        float strength = 1.0f;
    };
    std::shared_ptr<GrayscaleParam> param_;
};

