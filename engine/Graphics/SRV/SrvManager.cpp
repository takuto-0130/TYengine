#include "SrvManager.h"
#include "DirectXBasis.h"
#include "DirectXTex/d3dx12.h"
#include <cassert>

void SrvManager::Initialize(DirectXBasis* dxBasis, uint32_t maxDescriptors) {
    dxBasis_ = dxBasis;
    maxIndex_ = maxDescriptors;

    D3D12_DESCRIPTOR_HEAP_DESC desc{};
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.NumDescriptors = maxDescriptors;
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    HRESULT hr = dxBasis_->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
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

void SrvManager::CreateSRVforTexture2D(uint32_t index, ID3D12Resource* resource, DXGI_FORMAT format, uint32_t mipLevels) {
    assert(index < maxIndex_);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = mipLevels;

    D3D12_CPU_DESCRIPTOR_HANDLE handle = srvHeap_->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += index * descriptorSize_;

    dxBasis_->GetDevice()->CreateShaderResourceView(resource, &srvDesc, handle);
}

void SrvManager::CreateSRVforStructuredBuffer(uint32_t index, ID3D12Resource* resource, UINT elementCount, UINT elementSize) {
    assert(index < maxIndex_);

    D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Buffer.NumElements = elementCount;
    desc.Buffer.StructureByteStride = elementSize;
    desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

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

void SrvManager::BeginDraw() {
    //currentIndex_ = 0;
}


//#include "SrvManager.h"
//
//const uint32_t SrvManager::kMaxCount = 512;
//
//void SrvManager::Initialize(DirectXBasis* dxBasis)
//{
//	dxBasis_ = dxBasis;
//	// デスクリプタヒープの生成
//	descriptorHeap_ = dxBasis_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, kMaxCount, true);
//	// デスクリプタ1個分のサイズを取得して記録
//	descriptorSize_ = dxBasis_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//}
//
//uint32_t SrvManager::Allocate()
//{
//	assert(useIndex_ < kMaxCount);
//	int index = useIndex_;
//	// 次のために1進める
//	useIndex_++;
//	return index;
//}
//
//void SrvManager::BeginDraw()
//{
//	// SRV用のデスクリプタヒープを指定する
//	ID3D12DescriptorHeap* heaps[] = { descriptorHeap_.Get() };
//	dxBasis_->GetCommandList()->SetDescriptorHeaps(1, heaps);
//}
//
//bool SrvManager::CanAllocate() const
//{
//	return useIndex_ < kMaxCount;
//}
//
//D3D12_CPU_DESCRIPTOR_HANDLE SrvManager::GetCPUDescriptorHandle(uint32_t index)
//{
//	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
//	handleCPU.ptr += (descriptorSize_ * index);
//	return handleCPU;
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE SrvManager::GetGPUDescriptorHandle(uint32_t index)
//{
//	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
//	handleGPU.ptr += (descriptorSize_ * index);
//	return handleGPU;
//}
//
//void SrvManager::CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT format, UINT MipLevels)
//{
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	// 2D用のSRV設定
//	srvDesc.Format = format;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャとしてビューを作成
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // デフォルトのシェーダマッピング
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	srvDesc.Texture2D.MipLevels = MipLevels;
//	srvDesc.Texture2D.PlaneSlice = 0; // 通常、0に設定
//	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f; // 最小のレベルの詳細度をクランプ
//
//	dxBasis_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
//}
//
//void SrvManager::CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride)
//{
//	if (!pResource) {
//		Logger::Log("❌ CreateSRVforStructuredBuffer: pResource is nullptr!");
//		assert(false); // デバッグ時は止めよう
//		return;
//	}
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
//	// 構造化バッファ用のSRV設定
//	srvDesc.Format = DXGI_FORMAT_UNKNOWN; // 構造化バッファの場合、フォーマットは不明（UNKNOWN）
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; // バッファとしてビューを作成
//	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // デフォルトのシェーダマッピング
//	srvDesc.Buffer.FirstElement = 0; // バッファ内の最初の要素のインデックス
//	srvDesc.Buffer.NumElements = numElements; // バッファ内の要素数
//	srvDesc.Buffer.StructureByteStride = structureByteStride; // 構造体のバイト単位のサイズ
//	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE; // 特殊なフラグは無し
//
//	dxBasis_->GetDevice()->CreateShaderResourceView(pResource, &srvDesc, GetCPUDescriptorHandle(srvIndex));
//}
//
//void SrvManager::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex)
//{
//	dxBasis_->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameterIndex, GetGPUDescriptorHandle(srvIndex));
//}
