#pragma once
#include "PostEffectBase.h"
class RadialBlurEffect :
    public PostEffectBase
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

	void ImGuiUpdate() override;

    void Apply(RenderTexture* input) override;

public:
    // Setter
    void SetCenter(const Vector2& c) { param_->kCenter = c; }
    void SetBlurWidth(float w) { param_->kBlurWidth = w; }
    void SetNumSamples(int n) { param_->kNumSamples = n; }

    // Getter
    Vector2 GetCenter() const { return param_->kCenter; }
    float GetBlurWidth() const { return param_->kBlurWidth; }
    int GetNumSamples() const { return param_->kNumSamples; }

private:
	struct RadialBlurParam
	{
		Vector2 kCenter = { 0.5f, 0.5f };
		float kBlurWidth = 0.03f;
		int kNumSamples = 5;
	};
	std::shared_ptr<RadialBlurParam> param_;
};

