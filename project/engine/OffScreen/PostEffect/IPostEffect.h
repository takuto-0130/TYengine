#pragma once
#include "CopyPass.h"
#include "RenderTexture.h"

class DirectXBasis;
class SrvManager;

class IPostEffect 
{
public:
    virtual ~IPostEffect() = default;
    virtual void Initialize(DirectXBasis*, SrvManager*) = 0;
    virtual void Update() = 0;
    virtual void ImGuiUpdate() = 0;
    virtual void Apply(RenderTexture* input) = 0;
    virtual bool IsEnabled() const = 0;
    virtual void SetEnabled(bool enabled) = 0;
};
