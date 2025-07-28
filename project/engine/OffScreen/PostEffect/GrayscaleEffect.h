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

public:
    // Setter
    void SetStrength(float s) { param_->strength = s; }
    void SetTintColor(const Vector3& color) { param_->tintColor = color; }

    // Getter
    float GetStrength() const { return param_->strength; }
    Vector3 GetTintColor() const { return param_->tintColor; }

private:
    struct GrayscaleParam {
        float strength = 1.0f;
        Vector3 tintColor = { 1.0f, 1.0f, 1.0f };
    };
    std::shared_ptr<GrayscaleParam> param_;
};

