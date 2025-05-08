#pragma once
#include "IRenderer.h"
#include "Camera.h"
#include "SrvManager.h"
#include "struct.h"
#include <d3d12.h>
#include <wrl.h>

constexpr uint32_t kMaxBillboardInstance = 100;

class BillboardRenderer : public IRenderer {
public:
    BillboardRenderer(Camera* camera, SrvManager* srv);
    void SetTextureIndex(uint32_t index);
    void Draw(const std::list<Particle>& particles, ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso) override;

private:
    Camera* camera_;
    SrvManager* srv_;

    Microsoft::WRL::ComPtr<ID3D12Resource> instanceBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraCB_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialCB_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    uint32_t srvIndex_ = 0;
    uint32_t textureSrvIndex_ = 0;

    struct InstanceData {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Vector4 color;
    };
    InstanceData* mappedData_ = nullptr;

    struct CameraForGPU {
        Vector3 worldPosition;
        float pad;
    };
    CameraForGPU* mappedCamera_ = nullptr;

    Material* mappedMaterial_ = nullptr;
};