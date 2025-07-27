#pragma once
#include "PostEffectBase.h"

class GrayscaleEffect
	: public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

private:
    struct GrayscaleParam {
        float strength = 1.0f;
        Vector3 tintColor = { 1.0f, 1.0f, 1.0f };
    };
    std::shared_ptr<GrayscaleParam> param_;
};

