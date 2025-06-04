#include "CylinderParticle.h"
#include <numbers>

void CylinderParticle::CreateResources() {
    const uint32_t kDivide = 32;
    const float topR = 1.0f, bottomR = 1.0f, height = 2.0f;
    const float dTheta = 2.0f * std::numbers::pi_v<float> / kDivide;

    std::vector<VertexData> vertices;
    for (uint32_t i = 0; i < kDivide; ++i) {
        float a = i * dTheta;
        float b = (i + 1) * dTheta;
        Vector3 ta = { std::cos(a) * topR, height * 0.5f, std::sin(a) * topR };
        Vector3 tb = { std::cos(b) * topR, height * 0.5f, std::sin(b) * topR };
        Vector3 ba = { std::cos(a) * bottomR, -height * 0.5f, std::sin(a) * bottomR };
        Vector3 bb = { std::cos(b) * bottomR, -height * 0.5f, std::sin(b) * bottomR };
        Vector3 na = { std::cos(a), 0.0f, std::sin(a) };
        Vector3 nb = { std::cos(b), 0.0f, std::sin(b) };

        // uvを反転している
        vertices.push_back({ { ta.x, ta.y, ta.z, 1.0f }, { 1.0f, 1.0f }, na });
        vertices.push_back({ { ba.x, ba.y, ba.z, 1.0f }, { 1.0f, 0.0f }, na });
        vertices.push_back({ { tb.x, tb.y, tb.z, 1.0f }, { 0.0f, 1.0f }, nb });

        vertices.push_back({ { tb.x, tb.y, tb.z, 1.0f }, { 0.0f, 1.0f }, nb });
        vertices.push_back({ { ba.x, ba.y, ba.z, 1.0f }, { 1.0f, 0.0f }, na });
        vertices.push_back({ { bb.x, bb.y, bb.z, 1.0f }, { 0.0f, 0.0f }, nb });
    }

    vertexResource_ = dxBasis_->CreateBufferResource(sizeof(VertexData) * vertices.size());
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * vertices.size());

    std::string texturePath = "Resources/gradationLine.png";
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