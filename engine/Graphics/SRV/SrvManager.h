#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <cstdint>
#include <unordered_map>

class DirectXBasis;

class SrvManager {
public:
    void Initialize(DirectXBasis* dxBasis, uint32_t maxDescriptors = 256);

    uint32_t Allocate();
    bool CanAllocate() const;

    void CreateSRVforTexture2D(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels);
    void CreateSRVforStructuredBuffer(uint32_t index, ID3D12Resource* resource, UINT elementCount, UINT elementSize);

    void SetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex, uint32_t index);

    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) const;
    ID3D12DescriptorHeap* GetHeap() const { return srvHeap_.Get(); }

    void BeginDraw();

    void CreateSRVforTextureCube(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels);

private:
    DirectXBasis* dxBasis_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
    uint32_t descriptorSize_ = 0;
    uint32_t currentIndex_ = 0;
    uint32_t maxIndex_ = 0;
};

