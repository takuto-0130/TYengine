#pragma once
#include "PostEffectBase.h"
class DissolveEffect :
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
    void SetUseEdge(bool use) { param_->useEdge = use ? 1 : 0; }
    void SetEdgeColor(const Vector3& color) { param_->edgeColor = color; }

    // Getter
    float GetThreshold() const { return param_->threshold; }
    bool GetUseEdge() const { return param_->useEdge != 0; }
    Vector3 GetEdgeColor() const { return param_->edgeColor; }

private:
    struct DissolveParam {
        float threshold = 0.0f;
        int useEdge = 0;
        float padding[2];
        Vector3 edgeColor = { 0.5f, 0.5f, 0.5f };
    };
    std::shared_ptr<DissolveParam> param_;
};

