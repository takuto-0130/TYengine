#pragma once
#include "IPostEffect.h"
class DissolveEffect :
    public IPostEffect
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input, RenderTexture* output) override;

private:
    struct DissolveParam {
        float threshold = 0.0f;
        bool useEdge = false;
    };
    std::shared_ptr<DissolveParam> param_;
};

