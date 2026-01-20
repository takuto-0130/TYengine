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
    (void)hr;
    assert(SUCCEEDED(hr));

    rtvHeap_ = dxBasis->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);
    rtvHandle_ = dxBasis->GetCpuDescriptorHandle(rtvHeap_.Get(), dxBasis->GetDescriptorSizeRTV(), 0);

    dxBasis_->GetDevice()->CreateRenderTargetView(texture_.Get(), nullptr, rtvHandle_);

    srvIndex_ = srvManager_->Allocate();
    srvManager_->CreateSRVForTexture2D(srvIndex_, texture_.Get(), format, 1);

    currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

    CreateDepthStencil();
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

    if (depthBuffer_) {
        dxBasis_->GetCommandList()->ClearDepthStencilView(
            dsvHandle_,
            D3D12_CLEAR_FLAG_DEPTH,
            1.0f, 0,
            0, nullptr);
    }

    //dsvHandle_ = dxBasis_->GetDSVHandle();
    dxBasis_->GetCommandList()->OMSetRenderTargets(1, &rtvHandle_, FALSE, &dsvHandle_);

    // ビューポートとシザー設定
    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(width_);
    viewport.Height = static_cast<float>(height_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissorRect{};
    scissorRect.left = 0;
    scissorRect.top = 0;
    scissorRect.right = static_cast<LONG>(width_);
    scissorRect.bottom = static_cast<LONG>(height_);

    dxBasis_->GetCommandList()->RSSetViewports(1, &viewport);
    dxBasis_->GetCommandList()->RSSetScissorRects(1, &scissorRect);

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

void RenderTexture::TransitionDepthToSRV()
{
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = depthBuffer_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dxBasis_->GetCommandList()->ResourceBarrier(1, &barrier);
}

void RenderTexture::TransitionDepthToWrite()
{
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = depthBuffer_.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    dxBasis_->GetCommandList()->ResourceBarrier(1, &barrier);
}




D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetGPUHandle() const {
    return srvManager_->GetGPUDescriptorHandle(srvIndex_);
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetDepthSRVHandle() const
{
    return srvManager_->GetGPUDescriptorHandle(depthSRVIndex_);
}

void RenderTexture::CreateDepthStencil()
{
    // 深度バッファのリソース作成
    D3D12_RESOURCE_DESC depthDesc{};
    depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthDesc.Width = width_;
    depthDesc.Height = height_;
    depthDesc.DepthOrArraySize = 1;
    depthDesc.MipLevels = 1;
    depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_HEAP_PROPERTIES heapProps{};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE depthClear{};
    depthClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthClear.DepthStencil.Depth = 1.0f;
    depthClear.DepthStencil.Stencil = 0;

    HRESULT hr = dxBasis_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &depthDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClear,
        IID_PPV_ARGS(&depthBuffer_));
    (void)hr;
    assert(SUCCEEDED(hr));

    // 明示的に記述子を渡す（Typeless に対応）
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // ← Typeless に対応する具体的フォーマット
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Texture2D.MipSlice = 0;

    // DSV作成
    dsvHeap_ = dxBasis_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
    dsvHandle_ = dxBasis_->GetCpuDescriptorHandle(dsvHeap_.Get(), dxBasis_->GetDescriptorSizeDSV(), 0);
    dxBasis_->GetDevice()->CreateDepthStencilView(depthBuffer_.Get(), &dsvDesc, dsvHandle_);

    // SRV作成（HLSLから読み取り用）
    depthSRVIndex_ = srvManager_->Allocate();
    srvManager_->CreateSRVForTexture2D(
        depthSRVIndex_,
        depthBuffer_.Get(),
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS,
        1
    );
}
