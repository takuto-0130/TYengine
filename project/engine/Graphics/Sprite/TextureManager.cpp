#include "TextureManager.h"
#include "StringUtility.h"
#include <cassert>
#include <filesystem>

TextureManager* TextureManager::GetInstance() {
    static TextureManager instance;
    return &instance;
}

void TextureManager::Initialize(DirectXBasis* dxBasis, SrvManager* srvManager) {
    dxBasis_ = dxBasis;
    srvManager_ = srvManager;
    CreateDummyCubemap();
}

void TextureManager::LoadTexture(const std::string& filePath) {
    if (textureDatas_.contains(filePath)) return;

    assert(srvManager_ != nullptr);
    assert(dxBasis_ != nullptr);
    assert(srvManager_->CanAllocate());

    std::wstring filePathW = StringUtility::ConvertString(filePath);

    DirectX::ScratchImage image;
    HRESULT hr;
    if (filePathW.ends_with(L".dds"))
    {
        hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    }
    else
    {
        hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    }
    assert(SUCCEEDED(hr));

    DirectX::ScratchImage mipImages;
    if (DirectX::IsCompressed(image.GetMetadata().format))
    {
        mipImages = std::move(image);
    }
    else
    {
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
    }
    assert(SUCCEEDED(hr));

    uint32_t index = srvManager_->Allocate();

    TextureData textureData;
    textureData.metadata = mipImages.GetMetadata();
    textureData.resource = dxBasis_->CreateTextureResource(textureData.metadata);

    // 中間リソースを取得して保持（重要）
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer =
        dxBasis_->UploadTextureData(textureData.resource, mipImages);
    // 現在のフレームスロットに保持
    frameUploadBuffers_[currentFrameIndex_].emplace_back(std::move(uploadBuffer));

    textureData.srvIndex = index;
    textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(index);
    textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(index);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureData.metadata.format;
    if(textureData.metadata.IsCubemap())
    {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MostDetailedMip = 0;
        srvDesc.TextureCube.MipLevels = UINT_MAX;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
    }
    else
    {
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = static_cast<UINT>(textureData.metadata.mipLevels);
        srvDesc.Texture2D.PlaneSlice = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    }

    dxBasis_->GetDevice()->CreateShaderResourceView(
        textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);

    textureDatas_[filePath] = textureData;
    OutputDebugStringA(("Already loaded: " + filePath + "\n").c_str());
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath) {
    assert(textureDatas_.contains(filePath));
    return textureDatas_.at(filePath).srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	assert(srvManager_->CanAllocate());

	TextureData& textureData = textureDatas_[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath) {
    assert(textureDatas_.contains(filePath));
    return textureDatas_.at(filePath).metadata;
}

void TextureManager::NextFrame()
{
    currentFrameIndex_ = (currentFrameIndex_ + 1) % FrameCount;
    // 古いフレームのバッファを開放
    frameUploadBuffers_[currentFrameIndex_].clear();
}

void TextureManager::CreateDummyCubemap() {
    ID3D12Device* device = DirectXBasis::GetInstance()->GetDevice();
    ID3D12GraphicsCommandList* commandList = DirectXBasis::GetInstance()->GetCommandList();

    dummyCubemapIndex_ = srvManager_->Allocate();

    // 黒ピクセル (0,0,0,0)
    uint32_t pixel = 0x00000000;
    D3D12_SUBRESOURCE_DATA subresources[6]{};
    for (int i = 0; i < 6; ++i) {
        subresources[i].pData = &pixel;
        subresources[i].RowPitch = sizeof(pixel);
        subresources[i].SlicePitch = sizeof(pixel);
    }

    // リソース記述
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.DepthOrArraySize = 6;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&dummyCubemap_));
    assert(SUCCEEDED(hr));

    UINT64 uploadSize = GetRequiredIntermediateSize(dummyCubemap_.Get(), 0, 6);
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadSize);
    device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&dummyCubemapUploadBuffer_));

    UpdateSubresources(commandList, dummyCubemap_.Get(), dummyCubemapUploadBuffer_.Get(), 0, 0, 6, subresources);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        dummyCubemap_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);

    srvManager_->CreateSRVforTextureCube(dummyCubemapIndex_, dummyCubemap_.Get(), DXGI_FORMAT_R8G8B8A8_UNORM, 1);
}
