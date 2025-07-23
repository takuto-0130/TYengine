#pragma once
#include "CopyPass.h"

class DirectXBasis;
class SrvManager;
class RenderTexture;

class IPostEffect 
{
public:
    virtual ~IPostEffect() = default;
    virtual void Initialize(DirectXBasis*, SrvManager*) = 0;
    virtual void Update() = 0;
    virtual void Apply(RenderTexture* input, RenderTexture* output) = 0;

protected:
    DirectXBasis* dx_ = nullptr;
    CopyPass copyPass_;
};
