#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include "struct.h"

class DirectXBasis;
class SrvManager;

struct CopyPassParam {
    Vector2 offset;
    Vector2 scale;
};

struct BlurSettings
{
    Vector2 kCenter = { 0.5f, 0.5f };
    float kBlurWidth = 0.08f;
    int kNumSamples = 10;
};

class CopyPass {
public:
    void Initialize(DirectXBasis* dxBasis, SrvManager* srvMgr, const std::wstring& vsPath, const std::wstring& psPath);
    void Update();
    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);

private:
    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvMgr_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    Microsoft::WRL::ComPtr<ID3D12Resource> copyParamBuffer_;
    CopyPassParam* mappedParam_ = nullptr;

    //Microsoft::WRL::ComPtr<ID3D12Resource> BlurBuffer_;
    //BlurSettings* blurSettings_ = nullptr;
};

