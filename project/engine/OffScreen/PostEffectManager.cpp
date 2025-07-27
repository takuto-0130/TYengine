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

    // 列幅設定用のパラメータ
    const float buttonWidth = 36.0f;           // UP/DOWNボタンの幅
    const float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
    const float rightColumnWidth = buttonWidth * 2 + buttonSpacing; // 右列の固定幅

    // 2カラムテーブル: 左 = エフェクト設定, 右 = UP/DOWN ボタン
    if (ImGui::BeginTable("EffectTable", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit)) {

        // 列幅を設定（左は自動、右は固定幅）
        ImGui::TableSetupColumn("Effect", ImGuiTableColumnFlags_WidthStretch); // 左: 自動伸縮
        ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_WidthFixed, rightColumnWidth); // 右: 固定幅

        // Outline (固定項目)
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); // 左カラム
        ImGui::PushID(-1);
        ImGui::Checkbox("", &enabledOutline_);
        ImGui::SameLine();
        if (ImGui::TreeNode("Outline")) {
            outlinePass->ImGuiUpdate();
            ImGui::TreePop();
        }
        ImGui::PopID();

        ImGui::TableSetColumnIndex(1); // 右カラム (Outlineは上下移動不要)
        ImGui::TextDisabled("-");

        // effectStack_ のループ
        for (size_t i = 0; i < effectStack_.size(); ++i) {
            ImGui::TableNextRow();

            // --- 左カラム: エフェクト設定 ---
            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(static_cast<int>(i));

            // チェックボックス
            bool enabled = effectStack_[i].effect->IsEnabled();
            if (ImGui::Checkbox("", &enabled)) {
                effectStack_[i].effect->SetEnabled(enabled);
            }
            ImGui::SameLine();

            // エフェクト固有のUI
            effectStack_[i].effect->ImGuiUpdate();

            // --- 右カラム: 横並びの UP / DOWN ボタン ---
            ImGui::TableSetColumnIndex(1);

            bool hasUp = (i > 0);
            bool hasDown = (i + 1 < effectStack_.size());

            // UP ボタン or Dummy
            if (hasUp) {
                if (ImGui::Button("UP", ImVec2(buttonWidth, 0))) {
                    std::swap(effectStack_[i], effectStack_[i - 1]);
                }
            }
            else {
                ImGui::Dummy(ImVec2(buttonWidth, 0));
            }

            ImGui::SameLine();

            // DOWN ボタン or Dummy
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

        ImGui::EndTable();
    }

    ImGui::End();
#endif // _DEBUG
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
