#pragma once
#include "PostEffectBase.h"
class LuminanceBasedOutlineEffect :
    public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

public:
    // Setter
    void SetThreshold(float t) { param_->threshold = t; }
    void SetEdgeWidth(float w) { param_->edgeWidth = w; }
    void SetEdgeIntensity(float i) { param_->edgeIntensity = i; }
    void SetEdgeColor(const Vector3& c) { param_->edgeColor = c; }

    // Getter
    float GetThreshold() const { return param_->threshold; }
    float GetEdgeWidth() const { return param_->edgeWidth; }
    float GetEdgeIntensity() const { return param_->edgeIntensity; }
    Vector3 GetEdgeColor() const { return param_->edgeColor; }

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

