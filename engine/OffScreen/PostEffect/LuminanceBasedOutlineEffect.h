#pragma once
#include "PostEffectBase.h"
class LuminanceBasedOutlineEffect :
    public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input) override;

private:
};

