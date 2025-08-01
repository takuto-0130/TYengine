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

struct EmitterParam
{
    Vector3 position = { 0,0,0 };
    float emitRate = 10.0f;

    uint32_t emitCount = 5;
    float emitInterval = 0.5f;
    float padding[2];

    Vector3 direction = { 0,1,0 };
    float angle = std::numbers::pi_v<float> * 2.0f;

    float speedMin = 1.0f, speedMax = 2.0f;
    float lifeMin = 1.0f, lifeMax = 3.0f;

    Vector4 color = { 1,1,1,1 };
};

struct EmitterState {
    uint32_t emitRemaining = 15;
    uint32_t emitThisFrame = 0;
    float emitTimer = 0.0f;
    uint32_t emitMode = 0;    // 0=OFF, 1=LOOP, 2=ONESHOT, 3=SEQUENTIAL
};

class IParticleRenderer 
{
public:
    virtual ~IParticleRenderer() = default;
    virtual void Initialize(DirectXBasis* dx, SrvManager* srv, Camera* cam);
    virtual void Update();
    virtual void Draw();


    struct ParticleEmitter 
    {
        Transform transform
        {
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        };
        uint32_t count = 5;
        float frequency = 0.5f;
        float frequencyTime = 0.0f;
        Vector4 color = { 1,1,1,1 };
    };

    virtual void SetEmitter(ParticleEmitter& emitter) { emitter_ = emitter; }

    virtual void TriggerEmit();
protected:
    struct ParticleP
    {
        Transform transform;
        Vector3 velocity;
        Vector4 color;
        float lifeTime;
        float currentTime;
    };

    struct ParticleForGPU 
    {
        Matrix4x4 WVP;
        Matrix4x4 World;
        Vector4 color;

        // 追加 (Compute Shader用)
        Vector3 position;
        float pad0;
        Vector3 velocity;
        float pad1;
        float currentTime;
        float lifeTime;
        uint32_t alive = 0;
    };

    struct CameraForGPUP
    {
        Vector3 worldPosition;
    };

    struct VertexData 
    {
        Vector4 position;
        Vector2 texCoord;
        Vector3 normal;
    };
    struct Material
    {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvTransform;
    };

    struct CSParams
    {
        float deltaTime;
        uint32_t numParticles;
        uint32_t frameIndex = 0;
        float padding;
        Matrix4x4 cameraViewProj;
    };

    virtual void CreateResources() = 0;

    void CreateRootSignature();
    void LoadShader();
    void CreatePipelineState();

    void CreateComputeRootSignatureAndPSO();

    virtual ParticleP MakeNewParticle(std::mt19937& random, const ParticleEmitter& emitter);
    virtual std::list<ParticleP> Emit(std::mt19937& random);

protected:
    float kDeltaTime = 1.0f / 60.0f;
    
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
    uint32_t uavIndex_ = 0;
    uint32_t textureIndex_ = 0;

    ParticleForGPU* instancingData_ = nullptr;
    Material* materialData_ = nullptr;
    CameraForGPUP* cameraData_ = nullptr;
    VertexData* vertexData_ = nullptr;

    std::list<ParticleP> particles_;
    std::random_device seedGene_;
    uint32_t numInstance_ = 0;

    ParticleEmitter emitter_;

    Transform transform_{};

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> computeRootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState_;
    Microsoft::WRL::ComPtr<IDxcBlob> vsBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> psBlob_;

    D3D12_BLEND_DESC blendDesc_{};
    D3D12_RASTERIZER_DESC rasterizerDesc_{};


    CSParams csParams_;
    Microsoft::WRL::ComPtr<ID3D12Resource> csParamBuffer_;



    EmitterParam emitterParam_;
    Microsoft::WRL::ComPtr<ID3D12Resource> emitterBuffer_;

    EmitterState emitterState_;
    Microsoft::WRL::ComPtr<ID3D12Resource> emitterStateBuffer_;
    Microsoft::WRL::ComPtr<ID3D12Resource> emitterStateUploadBuffer_;
    uint32_t uavEmitterStateIndex_ = 0;
};
