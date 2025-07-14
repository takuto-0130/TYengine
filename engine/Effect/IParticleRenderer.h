#pragma once
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "Camera.h"
#include "TextureManager.h"
#include <struct.h>
#include <random>
#include <list>
#include <wrl.h>
#include <numbers>

class IParticleRenderer {
public:
    virtual ~IParticleRenderer() = default;
    virtual void Initialize(DirectXBasis* dx, SrvManager* srv, Camera* cam);
    virtual void Update();
    virtual void Draw();


    struct Emitter {
        Transform transform{
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        };
        uint32_t count = 5;
        float frequency = 0.5f;
        float frequencyTime = 0.0f;
        Vector4 color = { 1,1,1,1 };
    };

    virtual void SetEmitter(Emitter& emitter) { emitter_ = emitter; }

    virtual void TriggerEmit();
protected:
    struct ParticleP {
        Transform transform;
        Vector3 velocity;
        Vector4 color;
        float lifeTime;
        float currentTime;
    };

    struct ParticleForGPU {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Vector4 color;
    };

    struct CameraForGPUP {
        Vector3 worldPosition;
    };

    struct VertexData {
        Vector4 position;
        Vector2 texCoord;
        Vector3 normal;
    };
    struct Material {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
    };

    virtual void CreateResources() = 0;

    void CreateRootSignature();
    void LoadShader();
    void CreatePipelineState();
    virtual ParticleP MakeNewParticle(std::mt19937& random, const Emitter& emitter);
    virtual std::list<ParticleP> Emit(std::mt19937& random);

protected:
    const float kDeltaTime = 1.0f / 60.0f;
    
    static const uint32_t kMaxInstance = 100;

    bool useBillboard_ = true;

    bool useTrigger_ = true;

    uint32_t vertexCount_ = 0;

    DirectXBasis* dxBasis_ = nullptr;
    SrvManager* srvManager_ = nullptr;
    Camera* camera_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    uint32_t srvIndex_ = 0;
    uint32_t textureIndex_ = 0;

    ParticleForGPU* instancingData_ = nullptr;
    Material* materialData_ = nullptr;
    CameraForGPUP* cameraData_ = nullptr;
    VertexData* vertexData_ = nullptr;

    std::list<ParticleP> particles_;
    std::random_device seedGene_;
    uint32_t numInstance_ = 0;

    Emitter emitter_;

    Transform transform_{};

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    Microsoft::WRL::ComPtr<IDxcBlob> vsBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> psBlob_;

    D3D12_BLEND_DESC blendDesc_{};
    D3D12_RASTERIZER_DESC rasterizerDesc_{};
};
