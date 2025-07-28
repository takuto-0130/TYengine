#pragma once
#include "PostEffectBase.h"
class BoxFilterEffect :
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
    void SetTexelSize(const Vector2& texel) { param_->texelSize = texel; }

    // Getter
    int GetKernelSize() const { return param_->kernelSize; }
    Vector2 GetTexelSize() const { return param_->texelSize; }

private:
    struct BoxFilterParam
    {
        int kernelSize = 5;
        Vector2 texelSize = { 1.0f / 1280.0f, 1.0f / 720.0f }; // レンダリングターゲットの解像度
    };
    std::shared_ptr<BoxFilterParam> param_;
};

