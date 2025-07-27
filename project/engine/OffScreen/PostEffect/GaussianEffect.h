#pragma once
#include "PostEffectBase.h"
class GaussianEffect :
    public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

public:
    // Setter
    void SetKernelSize(int size) { param_->kernelSize = size; }
    void SetSigma(float sigma) { param_->sigma = sigma; }

    // Getter
    int GetKernelSize() const { return param_->kernelSize; }
    float GetSigma() const { return param_->sigma; }

private:
    struct GaussianParam 
    {
        int kernelSize = 3;
        float sigma = 5.0f;
    };
    std::shared_ptr<GaussianParam> param_;
};

