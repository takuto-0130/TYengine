#pragma once
#include "PostEffectBase.h"
class DissolveEffect :
    public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input) override;

private:
    struct DissolveParam {
        float threshold = 0.0f;
        bool useEdge = false;
    };
    std::shared_ptr<DissolveParam> param_;
};

