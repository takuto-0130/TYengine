#pragma once
#include <string>
#include <unordered_map>
#include <d3d12.h>
#include <wrl.h>
#include "struct.h"
#include "DirectXTex/DirectXTex.h"
#include "DirectXBasis.h"
#include "SrvManager.h"

static constexpr size_t FrameCount = 3;

class TextureManager {
public:
    static TextureManager* GetInstance();

    void Initialize(DirectXBasis* dxBasis, SrvManager* srvManager);

    void LoadTexture(const std::string& filePath);
    uint32_t GetTextureIndexByFilePath(const std::string& filePath);

    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

    SrvManager* GetSrvManager() const { return srvManager_; }

    const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

    void NextFrame();

private:
    struct TextureData {
        DirectX::TexMetadata metadata;
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        uint32_t srvIndex = 0;
        D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU{};
        D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU{};
    };

    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvManager_ = nullptr;
    std::unordered_map<std::string, TextureData> textureDatas_;

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> temporaryBuffers_;

    size_t currentFrameIndex_ = 0;
    std::array<std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>, FrameCount> frameUploadBuffers_;
};

