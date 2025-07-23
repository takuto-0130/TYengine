#include "PostEffectManager.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#include "CopyPass.h"

void PostEffectManager::Initialize(DirectXBasis* dx, SrvManager* srv)
{
    dxBasis_ = dx;
    srvMgr_ = srv;
}

void PostEffectManager::AddEffect(std::unique_ptr<IPostEffect> effect)
{
    effect->Initialize(dxBasis_, srvMgr_);
    effects_.emplace_back(std::move(effect));
}

void PostEffectManager::Update()
{
    for (auto& fx : effects_) 
    {
        fx->Update();
    }
}

void PostEffectManager::Apply(RenderTexture* source, RenderTexture* target) 
{
    RenderTexture* ping = source;
    RenderTexture* pong = tempRt_.get();

    for (size_t i = 0; i < effects_.size(); ++i) {
        bool isLast = (i == effects_.size() - 1);
        RenderTexture* dst = isLast ? target : pong;

        if (dst) {
            dst->BeginRender();
            effects_[i]->Apply(ping, dst);
            dst->EndRender();
        }
        else {
            // SwapChain に直接描画
            D3D12_CPU_DESCRIPTOR_HANDLE rtv = dxBasis_->GetBackBufferRTV();
            dxBasis_->GetCommandList()->OMSetRenderTargets(1, &rtv, FALSE, nullptr);
            effects_[i]->Apply(ping, nullptr);
        }

        std::swap(ping, pong);
    }
}

void PostEffectManager::SetTempRenderTexture(std::unique_ptr<RenderTexture> rt) 
{
    tempRt_ = std::move(rt);
}
