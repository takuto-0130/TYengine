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
}

void TextureManager::LoadTexture(const std::string& filePath) {
    if (textureDatas_.contains(filePath)) return;

    assert(srvManager_ != nullptr);
    assert(dxBasis_ != nullptr);
    assert(srvManager_->CanAllocate());

    std::wstring filePathW = StringUtility::ConvertString(filePath);

    DirectX::ScratchImage image;
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    DirectX::ScratchImage mipImages;
    hr = DirectX::GenerateMipMaps(
        image.GetImages(), image.GetImageCount(), image.GetMetadata(),
        DirectX::TEX_FILTER_SRGB, 0, mipImages);
    assert(SUCCEEDED(hr));

    uint32_t index = srvManager_->Allocate();

    TextureData textureData;
    textureData.metadata = mipImages.GetMetadata();
    textureData.resource = dxBasis_->CreateTextureResource(textureData.metadata);
    dxBasis_->UploadTextureData(textureData.resource, mipImages);

    textureData.srvIndex = index;
    textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(index);
    textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(index);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureData.metadata.format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = static_cast<UINT>(textureData.metadata.mipLevels);
    srvDesc.Texture2D.PlaneSlice = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

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
