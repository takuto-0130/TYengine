#include "TextureManager.h"
#include "DirectXBasis.h"

std::unique_ptr<TextureManager> TextureManager::instance_ = nullptr;
std::once_flag TextureManager::initInstanceFlag_;

uint32_t TextureManager::kSRVIndexTop_ = 1;


TextureManager* TextureManager::GetInstance()
{
	std::call_once(initInstanceFlag_, []() {
		instance_ = std::make_unique<TextureManager>();
		});
	return instance_.get();
}

void TextureManager::Initialize(DirectXBasis* dxBasis, SrvManager* srvManager)
{
	textureDatas_.reserve(DirectXBasis::kMaxSRVCount_);

	dxBasis_ = dxBasis;
	srvManager_ = srvManager;
}

void TextureManager::LoadTexture(const std::string& filePath)
{

	if (textureDatas_.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}

	// テクスチャ枚数上限チェック
	assert(srvManager_->CanAllocate());

	// テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	// テクスチャデータを追加
	textureDatas_[filePath] = TextureData();
	// 追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas_[filePath];

	textureData.srvIndex = srvManager_->Allocate();
	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxBasis_->CreateTextureResource(textureData.metadata);
	dxBasis_->UploadTextureData(textureData.resource, mipImages);

	// テクスチャデータの要素数番号をSRVのインデックスとする
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas_.size() - 1) + kSRVIndexTop_;

	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	// SRVの設定を行う
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureData.metadata.format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(textureData.metadata.mipLevels);
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// 設定をもとにSRVの生成
	dxBasis_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	if (textureDatas_.contains(filePath)) {
		// 読み込み済みなら要素番号を返す
		uint32_t textureIndex = textureDatas_.at(filePath).srvIndex;
		return textureIndex;
	}

	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	assert(srvManager_->CanAllocate());

	TextureData& textureData = textureDatas_[filePath];
	return textureData.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// 範囲外指定違反チェック
	assert(srvManager_->CanAllocate());

	TextureData& textureData = textureDatas_[filePath];
	return textureData.metadata;
}
