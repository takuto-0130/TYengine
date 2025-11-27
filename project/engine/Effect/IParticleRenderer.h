#pragma once
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "Camera.h"
#include "TextureManager.h"
#include "ParticleParam.h"
#include "IParticleBehaviour.h"
#include <struct.h>
#include <random>
#include <list>
#include <wrl.h>
#include <numbers>

class IParticleRenderer {
public:
    virtual ~IParticleRenderer();
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
        Vector3 velocity = { 0.0f, 0.0f, 0.0f };
        bool randomVel = false;
    };

    virtual void SetEmitter(Emitter& emitter) { emitter_ = emitter; }

    virtual void TriggerEmit();

    void SetBehaviour(std::unique_ptr<IParticleBehaviour> b)
    {
        behaviour_ = std::move(b);
    }

    struct ParticleP
    {
        Transform transform;
        Vector3 velocity;
        Vector4 color;
        float lifeTime;
        float currentTime;
    };
protected:

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
    virtual ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter);
    virtual std::list<ParticleParam> Emit(std::mt19937& random);

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
    uint32_t textureIndex_ = 0;

    ParticleForGPU* instancingData_ = nullptr;
    Material* materialData_ = nullptr;
    CameraForGPUP* cameraData_ = nullptr;
    VertexData* vertexData_ = nullptr;

    std::list<ParticleParam> particles_;
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

    std::unique_ptr<IParticleBehaviour> behaviour_;
};
