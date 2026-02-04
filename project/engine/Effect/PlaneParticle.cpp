#include "PlaneParticle.h"
#include "Random.h"
#include "Timer.h"
#include <numbers>

namespace TYEngine {
namespace Effect {

using namespace Utility;

void PlaneParticle::CreateResources() 
{
    // 板ポリゴンの頂点データ
    std::vector<VertexData> vertices = 
    {
        {{ 1, 1, 0, 1 }, {0, 0}, {0, 0, 1}},
        {{-1, 1, 0, 1 }, {1, 0}, {0, 0, 1}},
        {{ 1,-1, 0, 1 }, {0, 1}, {0, 0, 1}},
        {{ 1,-1, 0, 1 }, {0, 1}, {0, 0, 1}},
        {{-1, 1, 0, 1 }, {1, 0}, {0, 0, 1}},
        {{-1,-1, 0, 1 }, {1, 1}, {0, 0, 1}},
    };

    // 頂点バッファ作成
    vertexResource_ = dxBasis_->CreateBufferResource(sizeof(VertexData) * vertices.size());
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * vertices.size());

    // テクスチャロード（デフォルト：円）
    std::string texturePath = "Resources/Texture/circle.png";
    TextureManager::GetInstance()->LoadTexture(texturePath);
    textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);

    // インスタンシング用バッファ
    instancingResource_ = dxBasis_->CreateBufferResource(sizeof(ParticleForGPU) * kMaxInstance);
    instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
    srvIndex_ = srvManager_->Allocate();
    srvManager_->CreateSRVForStructuredBuffer(srvIndex_, instancingResource_.Get(), kMaxInstance, sizeof(ParticleForGPU));

    // マテリアル定数バッファ
    materialResource_ = dxBasis_->CreateBufferResource(256);
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    materialData_->color = { 1,1,1,1 };
    materialData_->enableLighting = true;
    materialData_->uvTransform = MakeIdentity4x4();

    // カメラ定数バッファ
    cameraResource_ = dxBasis_->CreateBufferResource(256);
    cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

    vertexCount_ = static_cast<uint32_t>(vertices.size());
}

ParticleParam PlaneParticle::MakeNewParticle(std::mt19937& random, const Emitter& emitter) 
{
    ParticleParam parti;
    parti.transform.scale = { emitter.transform.scale.x,emitter.transform.scale.y, emitter.transform.scale.z };
    parti.transform.rotate = { 0.f,0.f,0.f };
    parti.transform.translate = emitter.transform.translate;
    
    // ランダム速度を使用するかどうか
    if (emitter.randomVel)
    {
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        Vector3 rnd{ dist(random), dist(random), dist(random) };
        rnd = Normalize(rnd);
        float baseSpeed = Length(emitter.velocity);
        parti.velocity = rnd * baseSpeed;
    }
    else
    {
        parti.velocity = emitter.velocity;
    }

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
    parti.color = { 1, 1, 1, 1.0f };

    std::uniform_real_distribution<float> timeDist(0.7f, 0.7f);
    parti.lifeTime = timeDist(random);
    parti.currentTime = 0.0f;

    return parti;
}

} // namespace Effect
} // namespace TYEngine
