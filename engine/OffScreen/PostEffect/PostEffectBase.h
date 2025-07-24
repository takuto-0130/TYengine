#pragma once
#include "IPostEffect.h"

class PostEffectBase
    : public IPostEffect {
public:
    bool IsEnabled() const override { return enabled_; }
    void SetEnabled(bool enabled) override { enabled_ = enabled; }

protected:
    bool enabled_ = true;
    DirectXBasis* dx_ = nullptr;
    CopyPass copyPass_;
};
