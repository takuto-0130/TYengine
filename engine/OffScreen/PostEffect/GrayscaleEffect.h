#pragma once
#include "IPostEffect.h"

class CopyPass;

class GrayscaleEffect
	: public IPostEffect
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input, RenderTexture* output) override;

private:
    DirectXBasis* dx_ = nullptr;
    CopyPass copyPass_;
};

