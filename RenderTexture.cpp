#include "RenderTexture.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

void RenderTexture::Initialize(DirectXBasis* dxBasis, SrvManager* srvManager, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {
    dxBasis_ = dxBasis;
    srvManager_ = srvManager;
    width_ = width;
    height_ = height;
    format_ = format;
    clearColor_ = clearColor;

    D3D12_RESOURCE_DESC desc{};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = width;
    desc.Height = height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE clear{};
    clear.Format = format;
    clear.Color[0] = clearColor.x;
    clear.Color[1] = clearColor.y;
    clear.Color[2] = clearColor.z;
    clear.Color[3] = clearColor.w;

    HRESULT hr = dxBasis->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        &clear,
        IID_PPV_ARGS(&texture_));
    assert(SUCCEEDED(hr));

    rtvHeap_ = dxBasis->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
    rtvHandle_ = dxBasis->GetCpuDescriptorHandle(rtvHeap_.Get(), dxBasis->GetDescriptorSizeRTV(), 0);

    dxBasis_->GetDevice()->CreateRenderTargetView(texture_.Get(), nullptr, rtvHandle_);

    srvIndex_ = srvManager_->Allocate();
    srvManager_->CreateSRVforTexture2D(srvIndex_, texture_.Get(), format, 1);

    currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
}

void RenderTexture::BeginRender() {
    if (currentState_ != D3D12_RESOURCE_STATE_RENDER_TARGET) {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            texture_.Get(),
            currentState_,
            D3D12_RESOURCE_STATE_RENDER_TARGET);
        dxBasis_->GetCommandList()->ResourceBarrier(1, &barrier);
        currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxBasis_->GetDSVHandle();
    dxBasis_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle_, FALSE, &dsvHandle);
    float clearColor[] = { clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w };
    dxBasis_->GetCommandList()->ClearRenderTargetView(rtvHandle_, clearColor, 0, nullptr);
}

void RenderTexture::EndRender() {
    if (currentState_ != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            texture_.Get(),
            currentState_,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        dxBasis_->GetCommandList()->ResourceBarrier(1, &barrier);
        currentState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    }
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetGPUHandle() const {
    return srvManager_->GetGPUDescriptorHandle(srvIndex_);
}
