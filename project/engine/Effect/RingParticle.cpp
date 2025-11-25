#include "RingParticle.h"
#include "Timer.h"
#include <numbers>

void RingParticle::CreateResources() {
    const uint32_t kDivide = 32;
    const float outer = 0.4f, inner = 0.1f;
    const float dTheta = 2.0f * std::numbers::pi_v<float> / float(kDivide);

    std::vector<VertexData> vertices;
    for (uint32_t i = 0; i < kDivide; ++i) 
    {
        float angle = i * dTheta;
        float nextAngle = (i + 1) * dTheta;

        float cosA = std::cos(angle), sinA = std::sin(angle);
        float cosB = std::cos(nextAngle), sinB = std::sin(nextAngle);

        Vector3 outerA = { cosA * outer, sinA * outer, 0.0f };
        Vector3 outerB = { cosB * outer, sinB * outer, 0.0f };
        Vector3 innerA = { cosA * inner, sinA * inner, 0.0f };
        Vector3 innerB = { cosB * inner, sinB * inner, 0.0f };

        // 三角形① outerA, innerA, outerB
        vertices.push_back({ { outerA.x, outerA.y, outerA.z, 1.0f }, { 0.0f, 0.0f }, { 0, 0, 1 } });
        vertices.push_back({ { innerA.x, innerA.y, innerA.z, 1.0f }, { 0.0f, 1.0f }, { 0, 0, 1 } });
        vertices.push_back({ { outerB.x, outerB.y, outerB.z, 1.0f }, { 1.0f, 0.0f }, { 0, 0, 1 } });

        // 三角形② outerB, innerA, innerB
        vertices.push_back({ { outerB.x, outerB.y, outerB.z, 1.0f }, { 1.0f, 0.0f }, { 0, 0, 1 } });
        vertices.push_back({ { innerA.x, innerA.y, innerA.z, 1.0f }, { 0.0f, 1.0f }, { 0, 0, 1 } });
        vertices.push_back({ { innerB.x, innerB.y, innerB.z, 1.0f }, { 1.0f, 1.0f }, { 0, 0, 1 } });
    }

    // 以下リソース作成は共通（PlaneParticle同様）
    vertexResource_ = dxBasis_->CreateBufferResource(sizeof(VertexData) * vertices.size());
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    std::memcpy(vertexData_, vertices.data(), sizeof(VertexData) * vertices.size());

    std::string texturePath = "Resources/Texture/gradationLine.png";
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

ParticleParam RingParticle::MakeNewParticle(std::mt19937& random, const Emitter& emitter)
{
    ParticleParam parti;
    parti.transform.scale = emitter.transform.scale;
    parti.transform.rotate = emitter.transform.rotate;
    parti.transform.translate = emitter.transform.translate;
    parti.velocity = { 0.f,0.f,0.f };

    std::uniform_real_distribution<float> colorDist(1.0f, 1.0f);
    parti.color = { colorDist(random), colorDist(random), colorDist(random), 1.0f };

    std::uniform_real_distribution<float> timeDist(0.7f, 0.7f);
    parti.lifeTime = timeDist(random);
    parti.currentTime = 0.0f;

    return parti;
}

std::list<ParticleParam> RingParticle::Emit(std::mt19937& random)
{
    std::list<ParticleParam> result;
    for (uint32_t i = 0; i < emitter_.count; ++i) {
        Emitter emitter = emitter_;
        emitter.transform.scale = emitter.transform.scale * ((2.0f + float(i) * 2.0f) / 4.0f);
        result.push_back(MakeNewParticle(random, emitter));
    }
    return result;
}

//void RingParticle::Update() {
//    std::mt19937 random(seedGene_());
//
//    Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
//    Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorldMatrix());
//    billboardMatrix.m[3][0] = 0.0f;
//    billboardMatrix.m[3][1] = 0.0f;
//    billboardMatrix.m[3][2] = 0.0f;
//
//    numInstance_ = 0;
//
//    kDeltaTime = Timer::GetInstance()->GetDeltaTime();
//
//    emitter_.frequencyTime += kDeltaTime;
//    if (!useTrigger_ && emitter_.frequencyTime >= emitter_.frequency) {
//        particles_.splice(particles_.end(), Emit(random));
//        emitter_.frequencyTime -= emitter_.frequency;
//    }
//
//    for (auto it = particles_.begin(); it != particles_.end();) {
//        it->currentTime += kDeltaTime;
//        it->transform.translate += it->velocity * kDeltaTime;
//        float t = it->currentTime / it->lifeTime;
//        t = powf(t, 3.0f);
//        Vector3 scale = Vector3{ 1.0f,1.0f,1.0f };
//        scale = scale * t;
//
//        if (it->currentTime >= it->lifeTime) {
//            it = particles_.erase(it);
//            continue;
//        }
//
//        if (numInstance_ < kMaxInstance) {
//            Matrix4x4 world = MakeAffineMatrix(it->transform.scale + scale, it->transform.rotate, it->transform.translate);
//            if (useBillboard_) {
//                world = MakeScaleMatrix(it->transform.scale + scale)
//                    * billboardMatrix
//                    * MakeRotateZMatrix(it->transform.rotate.z)
//                    * MakeTranslateMatrix(it->transform.translate);
//            }
//            Matrix4x4 WVP = world * camera_->GetViewProjectionMatrix();
//            instancingData_[numInstance_].WVP = WVP;
//            instancingData_[numInstance_].World = world;
//            instancingData_[numInstance_].color = it->color;
//            instancingData_[numInstance_].color.w *= (1.0f - (it->currentTime / it->lifeTime));
//            ++numInstance_;
//        }
//        ++it;
//    }
//}
