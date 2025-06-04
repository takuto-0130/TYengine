#include "PlaneParticle.h"

void PlaneParticle::CreateResources() {
    std::vector<VertexData> vertices = {
        {{ 1, 1, 0, 1 }, {0, 0}, {0, 0, 1}},
        {{-1, 1, 0, 1 }, {1, 0}, {0, 0, 1}},
        {{ 1,-1, 0, 1 }, {0, 1}, {0, 0, 1}},
        {{ 1,-1, 0, 1 }, {0, 1}, {0, 0, 1}},
        {{-1, 1, 0, 1 }, {1, 0}, {0, 0, 1}},
        {{-1,-1, 0, 1 }, {1, 1}, {0, 0, 1}},
    };

    vertexResource_ = dxBasis_->CreateBufferResource(sizeof(VertexData) * vertices.size());
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * vertices.size());

    std::string texturePath = "Resources/circle.png";
    TextureManager::GetInstance()->LoadTexture(texturePath);
    textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);

    instancingResource_ = dxBasis_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxInstance);
    instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
    srvIndex_ = srvManager_->Allocate();
    srvManager_->CreateSRVforStructuredBuffer(srvIndex_, instancingResource_.Get(), kMaxInstance, sizeof(ParticleForGPU));

    materialResource_ = dxBasis_->CreateBufferResource(256);
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    materialData_->color = { 1,1,1,1 };
    materialData_->enableLighting = true;
    materialData_->uvTransform = MakeIdentity4x4();

    cameraResource_ = dxBasis_->CreateBufferResource(256);
    cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

    vertexCount_ = static_cast<uint32_t>(vertices.size());
}

PlaneParticle::ParticleP PlaneParticle::MakeNewParticle(std::mt19937& random, const Emitter& emitter) {
    ParticleP parti;
    std::uniform_real_distribution<float> distScale(0.6f, 1.4f);
    parti.transform.scale = { emitter.transform.scale.x,distScale(random), emitter.transform.scale.z};
    std::uniform_real_distribution<float> distRota(0.0f, 2.0f * std::numbers::pi_v<float>);
    parti.transform.rotate = { 0.f,0.f,distRota(random) };
    parti.transform.translate = emitter.transform.translate;
    parti.velocity = { 0.f,0.f,0.f };

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
    parti.color = { colorDist(random), colorDist(random), colorDist(random), 1.0f };

    std::uniform_real_distribution<float> timeDist(0.7f, 0.7f);
    parti.lifeTime = timeDist(random);
    parti.currentTime = 0.0f;

    return parti;
}
