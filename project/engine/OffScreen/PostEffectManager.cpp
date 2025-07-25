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

    outlinePass = std::make_unique<OutlinePass>();
    outlinePass->Initialize(dxBasis_, srvMgr_);
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

    const float windowWidth = ImGui::GetWindowContentRegionMax().x;
    const float buttonWidth = 48.0f;
    const float spacing = ImGui::GetStyle().ItemSpacing.x;

    ImGui::Checkbox("Outline", &enabledOutline_);

    for (size_t i = 0; i < effectStack_.size(); ++i) {
        ImGui::PushID(static_cast<int>(i));

        // チェックボックス（名前付き）
        bool enabled = effectStack_[i].effect->IsEnabled();
        if (ImGui::Checkbox(effectStack_[i].name.c_str(), &enabled)) {
            effectStack_[i].effect->SetEnabled(enabled);
        }

        // ボタンの有無
        bool hasUp = (i > 0);
        bool hasDown = (i + 1 < effectStack_.size());

        // 常に2ボタン分のスペースを確保（整列のため）
        float totalButtonWidth = buttonWidth * 2 + spacing;

        ImGui::SameLine(windowWidth - totalButtonWidth);

        // --- UPボタン or ダミー ---
        if (hasUp) {
            if (ImGui::Button("UP", ImVec2(buttonWidth, 0))) {
                std::swap(effectStack_[i], effectStack_[i - 1]);
            }
        }
        else {
            ImGui::Dummy(ImVec2(buttonWidth, 0));
        }

        ImGui::SameLine();

        // --- DOWNボタン or ダミー ---
        if (hasDown) {
            if (ImGui::Button("DOWN", ImVec2(buttonWidth, 0))) {
                std::swap(effectStack_[i], effectStack_[i + 1]);
            }
        }
        else {
            ImGui::Dummy(ImVec2(buttonWidth, 0));
        }

        ImGui::PopID();
    }

    ImGui::End();
#endif // _DEBUG
    outlinePass->Update();
    for (auto& fx : effectStack_)
    {
        fx.effect->Update();
    }
}

RenderTexture* PostEffectManager::ApplyOutline(RenderTexture* source)
{
    // ---------- アウトライン適用 ----------
    outlineRt_->BeginRender(); // 中間結果用

    source->TransitionDepthToSRV();

    outlinePass->SetDepthSrv(source->GetDepthSRVHandle());
    outlinePass->Draw(dxBasis_->GetCommandList(), source->GetGPUHandle());
    outlineRt_->EndRender();

    source->TransitionDepthToWrite();

    return outlineRt_.get();
}

void PostEffectManager::Apply(RenderTexture* source, RenderTexture* target) 
{
    RenderTexture* ping = source;
    RenderTexture* pong = tempRt_.get();

    if (enabledOutline_) ping = ApplyOutline(ping);

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

void PostEffectManager::SetOutlineRenderTexture(std::unique_ptr<RenderTexture> rt)
{
    outlineRt_ = std::move(rt);
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

void PostEffectManager::EffectAllDisable()
{
    SetOutlineEnabled(false);
	for (auto& entry : effectStack_) {
		entry.effect->SetEnabled(false);
	}
}
