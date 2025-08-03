#pragma once
#include "PostEffectBase.h"
class RandomEffect :
    public PostEffectBase
{
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

public:
    // Setter
    void SetT(float t) { param_->t = t; }
    void SetNoiseStrength(float s) { param_->noiseStrength = s; }
    void SetNoiseTint(const Vector3& c) { param_->noiseTint = c; }

    // Getter
    float GetT() const { return param_->t; }
    float GetNoiseStrength() const { return param_->noiseStrength; }
    Vector3 GetNoiseTint() const { return param_->noiseTint; }

private:
    struct RandomParam {
        float t = 1.0f;
        float noiseStrength = 0.5f;
        float padding[2];
        Vector3 noiseTint = { 0.3f, 0.3f, 0.3f };
    };
    std::shared_ptr<RandomParam> param_;
};

