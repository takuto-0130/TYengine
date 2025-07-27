#pragma once
#include "PostEffect/IPostEffect.h"
#include "OutlinePass.h"
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
    static PostEffectManager* GetInstance() {
        static PostEffectManager instance;
        return &instance;
    }

    // コピー禁止
    PostEffectManager(const PostEffectManager&) = delete;
    PostEffectManager& operator=(const PostEffectManager&) = delete;
    PostEffectManager(PostEffectManager&&) = delete;
    PostEffectManager& operator=(PostEffectManager&&) = delete;
private:
    PostEffectManager() = default;

public:
    void Initialize(DirectXBasis* dx, SrvManager* srv);

    void AddEffect(const std::string& name, std::shared_ptr<IPostEffect> effect);

    void Update();

    RenderTexture* ApplyOutline(RenderTexture* source);

    void Apply(RenderTexture* source, RenderTexture* target);

    void SetTempRenderTexture(std::unique_ptr<RenderTexture> rt);

    void SetOutlineRenderTexture(std::unique_ptr<RenderTexture> rt);

    void SetEffectEnabled(const std::string& name, bool enabled);
    void MoveEffect(const std::string& name, int newIndex); // 並べ替え

    void SetOutlineEnabled(bool enabled) { enabledOutline_ = enabled; }

    void EffectAllDisable();

    template<typename T>
    T* GetEffect(const std::string& name) {
        for (auto& e : effectStack_) {
            if (e.name == name) {
                return dynamic_cast<T*>(e.effect.get());
            }
        }
        return nullptr;
    }

private:
    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvMgr_ = nullptr;

    struct EffectEntry {
        std::string name;
        std::shared_ptr<IPostEffect> effect;
    };

    std::vector<EffectEntry> effectStack_;
    std::unique_ptr<RenderTexture> tempRt_; // ping-pong用

    std::unique_ptr<RenderTexture> outlineRt_; // outline用
    std::unique_ptr<OutlinePass> outlinePass;
    bool enabledOutline_ = true;

    std::unique_ptr<CopyImageEffect> copyImage_;
};

