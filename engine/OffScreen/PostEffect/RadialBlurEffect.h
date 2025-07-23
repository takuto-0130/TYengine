#pragma once
#include "IPostEffect.h"
class RadialBlurEffect :
    public IPostEffect
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv) override;

    void Update() override;

    void Apply(RenderTexture* input, RenderTexture* output) override;

private:
	struct RadialBlurParam
	{
		Vector2 kCenter = { 0.5f, 0.5f };
		float kBlurWidth = 0.03f;
		int kNumSamples = 5;
	};
	std::shared_ptr<RadialBlurParam> param_;
};

