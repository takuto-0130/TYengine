#include "PlaneParticle.h"
#include "Timer.h"
#include <numbers>

#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void PlaneParticle::TriggerEmit()
{
    emitterStateUploadBuffer_.Reset();

    if (emitterState_.emitMode == 2)
    {
        emitterState_.emitRemaining = emitterParam_.emitCount;
    }
    emitterState_.emitThisFrame = 0;
    emitterState_.emitTimer = 0.0f;
    // モードをONESHOTに
    //emitterState_.emitMode = 3;

    // 2. 一時アップロード用バッファ
    emitterStateUploadBuffer_ = dxBasis_->CreateBufferResource(
        sizeof(EmitterState),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );

    // 3. データ書き込み
    void* mapped = nullptr;
    emitterStateUploadBuffer_->Map(0, nullptr, &mapped);
    memcpy(mapped, &emitterState_, sizeof(EmitterState));
    emitterStateUploadBuffer_->Unmap(0, nullptr);

    // 4. コマンドでDEFAULTヒープのemitterStateBuffer_にコピー
    auto cmd = dxBasis_->GetCommandList();

    // コピー先をCopyDestに遷移
    D3D12_RESOURCE_BARRIER barrierBegin = CD3DX12_RESOURCE_BARRIER::Transition(
        emitterStateBuffer_.Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_COPY_DEST
    );
    cmd->ResourceBarrier(1, &barrierBegin);

    cmd->CopyResource(emitterStateBuffer_.Get(), emitterStateUploadBuffer_.Get());

    // UAVに戻す
    D3D12_RESOURCE_BARRIER barrierEnd = CD3DX12_RESOURCE_BARRIER::Transition(
        emitterStateBuffer_.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS
    );
    cmd->ResourceBarrier(1, &barrierEnd);
}

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

    std::string texturePath = "Resources/Texture/circle.png";
    TextureManager::GetInstance()->LoadTexture(texturePath);
    textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(texturePath);

    instancingResource_ = dxBasis_->CreateBufferResource(
        sizeof(ParticleForGPU) * kMaxInstance,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
    );
    srvIndex_ = srvManager_->Allocate();
    srvManager_->CreateSRVforStructuredBuffer(srvIndex_, instancingResource_.Get(), kMaxInstance, sizeof(ParticleForGPU));

    uavIndex_ = srvManager_->Allocate();
    srvManager_->CreateUAVforStructuredBuffer(uavIndex_, instancingResource_.Get(), kMaxInstance, sizeof(ParticleForGPU));


    emitterStateBuffer_ = dxBasis_->CreateBufferResource(
        sizeof(EmitterState),
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

    uavEmitterStateIndex_ = srvManager_->Allocate();
    srvManager_->CreateUAVforStructuredBuffer(
        uavEmitterStateIndex_, emitterStateBuffer_.Get(), 1, sizeof(EmitterState));



    materialResource_ = dxBasis_->CreateBufferResource(256);
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    materialData_->color = { 1,1,1,1 };
    materialData_->enableLighting = true;
    materialData_->uvTransform = MakeIdentity4x4();

    cameraResource_ = dxBasis_->CreateBufferResource(256);
    cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

    vertexCount_ = static_cast<uint32_t>(vertices.size());


    csParamBuffer_ = dxBasis_->CreateBufferResource(
        sizeof(CSParams),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );

    // Mapして初期値を設定
    void* mapped = nullptr;
    csParamBuffer_->Map(0, nullptr, &mapped);
    memcpy(mapped, &csParams_, sizeof(CSParams));
    csParamBuffer_->Unmap(0, nullptr);

    CreateComputeRootSignatureAndPSO();
}

PlaneParticle::ParticleP PlaneParticle::MakeNewParticle(std::mt19937& random, const ParticleEmitter& emitter) {
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

void PlaneParticle::Update() 
{
#ifdef _DEBUG
    ImGui::Begin("Particle");


    if (ImGui::Button("Mode:LOOP"))
    {
        emitterState_.emitMode = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Mode:ONESHOT"))
    {
        emitterState_.emitMode = 2;
    }
    ImGui::SameLine();
    if (ImGui::Button("Mode:SEQUENTIAL"))
    {
        emitterState_.emitMode = 3;
    }
    int a = emitterParam_.emitCount;
    ImGui::DragInt("1frame/count", &a, 1.0f, 1, 1000000);
    emitterParam_.emitCount = a;

    int b = emitterState_.emitRemaining;
    ImGui::DragInt("SEQUENTIAL/MaxCount", &b, 1.0f, 1, 1000000);
    emitterState_.emitRemaining = b;

    ImGui::DragFloat("Interval", &emitterParam_.emitInterval, 0.1f, 0.1f, 5.0f);

    if (ImGui::Button("Apply/Emit"))
    {
        TriggerEmit();
    }
    ImGui::End();
#endif // _DEBUG



   auto cmd = dxBasis_->GetCommandList();

    ID3D12DescriptorHeap* heaps[] = { srvManager_->GetHeap() };
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);

    cmd->SetComputeRootSignature(computeRootSignature_.Get());
    cmd->SetPipelineState(computePipelineState_.Get());

    // UAV (particles)
    srvManager_->SetComputeRootDescriptorTable(cmd, 0, uavIndex_);

    // CSParams 更新
    csParams_.deltaTime = Timer::GetInstance()->GetDeltaTime();
    csParams_.numParticles = kMaxInstance;
    csParams_.cameraViewProj = camera_->GetViewProjectionMatrix();
    csParams_.frameIndex++;

    void* mapped = nullptr;
    csParamBuffer_->Map(0, nullptr, &mapped);
    memcpy(mapped, &csParams_, sizeof(CSParams));
    csParamBuffer_->Unmap(0, nullptr);
    cmd->SetComputeRootConstantBufferView(2, csParamBuffer_->GetGPUVirtualAddress());

    // EmitterParam 更新
    void* mappedEmitter = nullptr;
    emitterBuffer_->Map(0, nullptr, &mappedEmitter);
    memcpy(mappedEmitter, &emitterParam_, sizeof(EmitterParam));
    emitterBuffer_->Unmap(0, nullptr);
    cmd->SetComputeRootConstantBufferView(3, emitterBuffer_->GetGPUVirtualAddress());

    // UAV (emitterState)
    srvManager_->SetComputeRootDescriptorTable(cmd, 1, uavEmitterStateIndex_);

    // Dispatch
    cmd->Dispatch((kMaxInstance + 255) / 256, 1, 1);

    // UAV Barrier
    D3D12_RESOURCE_BARRIER barriers[] = {
        CD3DX12_RESOURCE_BARRIER::UAV(instancingResource_.Get()),
        CD3DX12_RESOURCE_BARRIER::UAV(emitterStateBuffer_.Get())
    };
    cmd->ResourceBarrier(_countof(barriers), barriers);

    numInstance_ = kMaxInstance;
}

void PlaneParticle::Draw()
{
    if (numInstance_ == 0) return;

    auto cmd = dxBasis_->GetCommandList();

    ID3D12DescriptorHeap* heaps[] = { srvManager_->GetHeap() };
    cmd->SetDescriptorHeaps(_countof(heaps), heaps);

    cmd->SetGraphicsRootSignature(rootSignature_.Get());
    cmd->SetPipelineState(pipelineState_.Get());

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &vertexBufferView_);

    srvManager_->SetGraphicsRootDescriptorTable(cmd, 1, srvIndex_);
    srvManager_->SetGraphicsRootDescriptorTable(cmd, 2, textureIndex_);
    cmd->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    cmd->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());

    cmd->DrawInstanced(vertexCount_, numInstance_, 0, 0);

    /*if (emitterStateUploadBuffer_)
    {
        emitterStateUploadBuffer_.Reset();
    }*/
}
