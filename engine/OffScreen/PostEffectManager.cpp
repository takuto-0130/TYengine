#include "PostEffectManager.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "CopyPass.h"

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void PostEffectManager::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dxBasis_ = dx;
    srvMgr_ = srv;
    copyImage_ = std::make_unique<CopyImageEffect>();
    copyImage_->Initialize(dxBasis_, srvMgr_);
}

void PostEffectManager::AddEffect(const std::string& name, std::shared_ptr<IPostEffect> effect)
{
    effect->Initialize(dxBasis_, srvMgr_);
    EffectEntry effectEntry = {
        name,
        std::move(effect)
    };
    effectStack_.emplace_back(std::move(effectEntry));
}

void PostEffectManager::Update()
{
#ifdef _DEBUG
    ImGui::Begin("PostEffectManager");
    for (size_t i = 0; i < effectStack_.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));

        bool enabled = effectStack_[i].effect->IsEnabled();
        if (ImGui::Checkbox(effectStack_[i].name.c_str(), &enabled)) {
            effectStack_[i].effect->SetEnabled(enabled);
        }

        ImGui::SameLine();
        if (i > 0 && ImGui::Button("UP")) {
            std::swap(effectStack_[i], effectStack_[i - 1]);
        }

        ImGui::SameLine();
        if (i + 1 < effectStack_.size() && ImGui::Button("DOWN")) {
            std::swap(effectStack_[i], effectStack_[i + 1]);
        }

        ImGui::PopID();
    }
    ImGui::End();
#endif // _DEBUG

    for (auto& fx : effectStack_)
    {
        fx.effect->Update();
    }
}

void PostEffectManager::Apply(RenderTexture* source, RenderTexture* target) 
{
    RenderTexture* ping = source;
    RenderTexture* pong = tempRt_.get();

    // 有効なエフェクトだけ抽出
    std::vector<IPostEffect*> enabledEffects;
    for (auto& e : effectStack_) {
        if (e.effect->IsEnabled()) {
            enabledEffects.push_back(e.effect.get());
        }
    }

    const size_t count = enabledEffects.size();
    if (count == 0)
    {
        D3D12_CPU_DESCRIPTOR_HANDLE rtv = dxBasis_->GetBackBufferRTV();
        dxBasis_->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
        copyImage_->Apply(ping);
    }
    else
    {
        for (size_t i = 0; i < enabledEffects.size(); ++i) {
            if (!enabledEffects[i]->IsEnabled()) continue;
            bool isLast = (i == enabledEffects.size() - 1);
            RenderTexture* dst = isLast ? target : pong;

            if (dst) {
                dst->BeginRender();
                enabledEffects[i]->Apply(ping);
                dst->EndRender();
            }
            else {
                // SwapChain に直接描画
                D3D12_CPU_DESCRIPTOR_HANDLE rtv = dxBasis_->GetBackBufferRTV();
                dxBasis_->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
                enabledEffects[i]->Apply(ping);
            }

            std::swap(ping, pong);
        }
    }
}

void PostEffectManager::SetTempRenderTexture(std::unique_ptr<RenderTexture> rt) 
{
    tempRt_ = std::move(rt);
}

void PostEffectManager::SetEffectEnabled(const std::string& name, bool enabled)
{
    for (auto& entry : effectStack_) {
        if (entry.name == name) {
            entry.effect->SetEnabled(enabled);
            return;
        }
    }
}

void PostEffectManager::MoveEffect(const std::string& name, int newIndex)
{
    auto it = std::find_if(effectStack_.begin(), effectStack_.end(),
        [&](const EffectEntry& e) { return e.name == name; });
    if (it == effectStack_.end() || newIndex < 0 || newIndex >= effectStack_.size()) return;

    EffectEntry entry = *it;
    effectStack_.erase(it);
    effectStack_.insert(effectStack_.begin() + newIndex, entry);
}
