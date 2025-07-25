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
    struct LuminanceOutlineParam
    {
        float threshold = 0.1f;
        float edgeWidth = 1.0f;
        float edgeIntensity = 1.0f;
        float padding;
        Vector3 edgeColor = { 0.0f, 0.0f, 0.0f };
    };
    std::shared_ptr<LuminanceOutlineParam> param_;
};

