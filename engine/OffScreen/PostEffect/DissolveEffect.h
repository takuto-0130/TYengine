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
        int useEdge = 0;
        float padding[2];
        Vector3 edgeColor = { 0.5f, 0.5f, 0.5f };
    };
    std::shared_ptr<DissolveParam> param_;
};

