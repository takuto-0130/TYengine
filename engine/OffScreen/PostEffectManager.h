#pragma once
#include "PostEffect/IPostEffect.h"
#include <vector>
#include <memory>

class DirectXBasis;
class SrvManager;
class RenderTexture;
class IPostEffect;

class PostEffectManager
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv);

    void AddEffect(std::unique_ptr<IPostEffect> effect);

    void Update();

    void Apply(RenderTexture* source, RenderTexture* target);

    void SetTempRenderTexture(std::unique_ptr<RenderTexture> rt);

private:
    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvMgr_ = nullptr;
    std::vector<std::unique_ptr<IPostEffect>> effects_;
    std::unique_ptr<RenderTexture> tempRt_; // ping-pong用
};

