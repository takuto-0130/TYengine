#pragma once
#include "IPostEffect.h"

class VignetteEffect
    : public IPostEffect
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input, RenderTexture* output) override;

private:
};

