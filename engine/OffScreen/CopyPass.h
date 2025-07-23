#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>
#include <memory>
#include "DirectXBasis.h"
#include "struct.h"

class SrvManager;

struct CopyPassParam {
    Vector2 offset;
    Vector2 scale;
};

struct ExtraBuffer {
    UINT registerIndex;
    Microsoft::WRL::ComPtr<ID3D12Resource> resource;
    void* mappedPtr;
};

class CopyPass {
public:
    void Initialize(DirectXBasis* dxBasis, SrvManager* srvMgr, const std::wstring& vsPath, const std::wstring& psPath);
    void Update();
    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle);

    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, bool toSwapChain);

    /// <summary>
    /// 個別の定数バッファを追加
    /// </summary>
    /// <typeparam name="T">追加したいバッファの型</typeparam>
    /// <param name="registerIndex">ルートパラメーターのインデックス（3or4）</param>
    /// <param name="defaultValue">初期値</param>
    /// <returns></returns>
    template<typename T>
    std::shared_ptr<T> AddExtraConstantBuffer(UINT registerIndex, const T& defaultValue = {}) {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource = dxBasis_->CreateBufferResource(sizeof(T));
        void* mapped = nullptr;
        CD3DX12_RANGE readRange(0, 0);
        resource->Map(0, &readRange, &mapped);
        memcpy(mapped, &defaultValue, sizeof(T));

        extraBuffers_.push_back({ registerIndex, resource, mapped });

        // shared_ptr で mappedPtr を外部に渡す（resource の寿命に従う）
        return std::shared_ptr<T>(reinterpret_cast<T*>(mapped), [resource](T*) {});
    }

private:
    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvMgr_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    Microsoft::WRL::ComPtr<ID3D12Resource> copyParamBuffer_;
    CopyPassParam* mappedParam_ = nullptr;

    std::vector<ExtraBuffer> extraBuffers_;
};

