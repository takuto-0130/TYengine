#include "SrvManager.h"
#include "DirectXBasis.h"
#include <cassert>

namespace TYEngine {
namespace Graphics {

using namespace Core; // For DirectXBasis

void SrvManager::Initialize(DirectXBasis* dxBasis, uint32_t maxDescriptors) {
    dxBasis_ = dxBasis;
    maxIndex_ = maxDescriptors;

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = maxDescriptors;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = dxBasis_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
    (void)hr;
    assert(SUCCEEDED(hr));

    descriptorSize_ = dxBasis_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

uint32_t SrvManager::Allocate() {
    assert(currentIndex_ < maxIndex_);
    return currentIndex_++;
}

bool SrvManager::CanAllocate() const {
    return currentIndex_ < maxIndex_;
}

void SrvManager::CreateSRVForTexture2D(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels) {
    assert(index < maxIndex_);

    // SRV記述子設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = mipLevels;

    // ヒープ上の場所を計算してSRV作成を行う
    D3D12_CPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;

    dxBasis_->GetDevice()->CreateShaderResourceView(resource, &srvDesc, handle);
}

void SrvManager::CreateSRVForStructuredBuffer(uint32_t index, ID3D12Resource* resource, UINT elementCount, UINT elementSize) {
    assert(index < maxIndex_);

    // StructuredBuffer用SRV記述子
    D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Buffer.NumElements = elementCount;
    desc.Buffer.StructureByteStride = elementSize;
    desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    // 作成
    D3D12_CPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;

    dxBasis_->GetDevice()->CreateShaderResourceView(resource, &desc, handle);
}

void SrvManager::SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex, uint32_t index) {
    D3D12_GPU_DESCRIPTOR_HANDLE handle = GetGPUDescriptorHandle(index);
    cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, handle);
}

D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index) const {
    assert(index < maxIndex_);
    D3D12_GPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index) const {
    assert(index < maxIndex_);
    D3D12_CPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandleFromGpu(D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) const
{
    size_t offset = gpuHandle.ptr - srvHeap_->GetGPUDescriptorHandleForHeapStart().ptr;
    size_t index = offset / descriptorSize_;
    return GetCPUDescriptorHandle(static_cast<uint32_t>(index));
}

void SrvManager::CreateSRVForTextureCube(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels) {
    assert(index < maxIndex_);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.TextureCube.MipLevels = mipLevels;
    srvDesc.TextureCube.MostDetailedMip = 0;
    srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

    D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUDescriptorHandle(index);
    // Cubemap用SRV作成
    dxBasis_->GetDevice()->CreateShaderResourceView(resource, &srvDesc, handle);
}

} // namespace Graphics
} // namespace TYEngine
