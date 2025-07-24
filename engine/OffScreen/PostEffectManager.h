#pragma once
#include "PostEffect/IPostEffect.h"
#include "CopyImage.h"
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

    void AddEffect(const std::string& name, std::shared_ptr<IPostEffect> effect);

    void Update();

    void Apply(RenderTexture* source, RenderTexture* target);

    void SetTempRenderTexture(std::unique_ptr<RenderTexture> rt);

    void SetEffectEnabled(const std::string& name, bool enabled);
    void MoveEffect(const std::string& name, int newIndex); // 並べ替え

private:
    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvMgr_ = nullptr;

    struct EffectEntry {
        std::string name;
        std::shared_ptr<IPostEffect> effect;
    };

    std::vector<EffectEntry> effectStack_;
    std::unique_ptr<RenderTexture> tempRt_; // ping-pong用
    std::unique_ptr<CopyImageEffect> copyImage_;
};

