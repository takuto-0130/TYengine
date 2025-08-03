#pragma once
#include "PostEffectBase.h"

class VignetteEffect
    : public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

public:
    // Setter
    void SetIntensity(float i) { param_->intensity = i; }
    void SetPower(float p) { param_->power = p; }
    void SetColor(const Vector3& c) { param_->vignetteColor = c; }

    // Getter
    float GetIntensity() const { return param_->intensity; }
    float GetPower() const { return param_->power; }
    Vector3 GetColor() const { return param_->vignetteColor; }

private:
    struct VignetteParam
    {
        float intensity = 16.0f;
        float power = 0.8f;
        float padding[2];
        Vector3 vignetteColor = { 0.0f, 0.0f, 0.0f };
    };
    std::shared_ptr<VignetteParam> param_;
};

