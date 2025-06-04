#include "PlaneParticle.h"
#include <numbers>
#include "operatorOverload.h"

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
    std::uniform_real_distribution<float> distScale(0.2f, 0.8f);
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

void PlaneParticle::Update() {
    std::mt19937 random(seedGene_());

    Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
    Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorldMatrix());
    billboardMatrix.m[3][0] = 0.0f;
    billboardMatrix.m[3][1] = 0.0f;
    billboardMatrix.m[3][2] = 0.0f;

    numInstance_ = 0;

    emitter_.frequencyTime += kDeltaTime;
    if (!useTrigger_ && emitter_.frequencyTime >= emitter_.frequency) {
        particles_.splice(particles_.end(), Emit(random));
        emitter_.frequencyTime -= emitter_.frequency;
    }

    for (auto it = particles_.begin(); it != particles_.end();) {
        it->currentTime += kDeltaTime;
        it->transform.translate += it->velocity * kDeltaTime;
        float t = it->currentTime / it->lifeTime;
        t = powf(t, 3.0f);
        float scaleF = 2.0f;
        scaleF = scaleF * t;
        Vector3 scale = it->transform.scale;
        scale.y += scaleF;

        if (it->currentTime >= it->lifeTime) {
            it = particles_.erase(it);
            continue;
        }

        if (numInstance_ < kMaxInstance) {
            Matrix4x4 world = MakeAffineMatrix(scale, it->transform.rotate, it->transform.translate);
            if (useBillboard_) {
                world = MakeScaleMatrix(scale)
                    * billboardMatrix
                    * MakeRotateZMatrix(it->transform.rotate.z)
                    * MakeTranslateMatrix(it->transform.translate);
            }
            Matrix4x4 WVP = world * camera_->GetViewProjectionMatrix();
            instancingData_[numInstance_].WVP = WVP;
            instancingData_[numInstance_].World = world;
            instancingData_[numInstance_].color = it->color;
            instancingData_[numInstance_].color.w *= (1.0f - (it->currentTime / it->lifeTime));
            ++numInstance_;
        }
        ++it;
    }
}
