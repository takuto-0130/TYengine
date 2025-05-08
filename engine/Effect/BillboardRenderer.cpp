#include "BillboardRenderer.h"
#include "DirectXBasis.h"
#include "operatorOverload.h"
#include <cassert>
#include <numbers>
#include <cstring>

namespace {
    uint32_t AlignTo256(uint32_t size) {
        return (size + 255) & ~255;
    }
}

BillboardRenderer::BillboardRenderer(Camera* camera, SrvManager* srv)
    : camera_(camera), srv_(srv) {
    auto dx = DirectXBasis::GetInstance();

    instanceBuffer_ = dx->CreateBufferResource(sizeof(InstanceData) * kMaxBillboardInstance);
    instanceBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedData_));

    srvIndex_ = srv_->Allocate();
    srv_->CreateSRVforStructuredBuffer(srvIndex_, instanceBuffer_.Get(), kMaxBillboardInstance, sizeof(InstanceData));

    VertexData quadVertices[] = {
        { {  1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { {  1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
        { { -1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
    };
    vertexBuffer_ = dx->CreateBufferResource(sizeof(quadVertices));
    void* mapped = nullptr;
    vertexBuffer_->Map(0, nullptr, &mapped);
    std::memcpy(mapped, quadVertices, sizeof(quadVertices));

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(quadVertices);
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    cameraCB_ = dx->CreateBufferResource(AlignTo256(sizeof(CameraForGPU)));
    cameraCB_->Map(0, nullptr, reinterpret_cast<void**>(&mappedCamera_));

    materialCB_ = dx->CreateBufferResource(AlignTo256(sizeof(Material)));
    materialCB_->Map(0, nullptr, reinterpret_cast<void**>(&mappedMaterial_));
    mappedMaterial_->color = { 1, 1, 1, 1 };
    mappedMaterial_->enableLighting = true;
    mappedMaterial_->uvTransform = MakeIdentity4x4();
}

void BillboardRenderer::SetTextureIndex(uint32_t index) {
    textureSrvIndex_ = index;
}

void BillboardRenderer::Draw(const std::list<Particle>& particles, ID3D12GraphicsCommandList* cmdList, ID3D12RootSignature* rootSig, ID3D12PipelineState* pso) {
    std::vector<const Particle*> liveParticles;
    for (const auto& p : particles) {
        if (p.currentTime < p.lifeTime) {
            liveParticles.push_back(&p);
        }
    }

    cmdList->SetGraphicsRootSignature(rootSig);
    cmdList->SetPipelineState(pso);

    cmdList->SetGraphicsRootConstantBufferView(0, materialCB_->GetGPUVirtualAddress());

    Matrix4x4 viewProj = camera_->GetViewProjectionMatrix();
    Matrix4x4 billboard = MakeRotateYMatrix(std::numbers::pi_v<float>) * camera_->GetWorldMatrix();
    billboard.m[3][0] = billboard.m[3][1] = billboard.m[3][2] = 0.0f;

    uint32_t count = 0;
    for (const auto* p : liveParticles) {
        if (count >= kMaxBillboardInstance) break;
        Matrix4x4 world = MakeAffineMatrix(p->transform.scale, p->transform.rotate, p->transform.translate);
        world = world * billboard;
        mappedData_[count].WVP = world * viewProj;
        mappedData_[count].World = world;
        mappedData_[count].color = p->color;
        ++count;
    }

    mappedCamera_->worldPosition = camera_->GetTranslate();

    srv_->BeginDraw();
    cmdList->SetGraphicsRootDescriptorTable(1, srv_->GetGPUDescriptorHandle(srvIndex_));
    cmdList->SetGraphicsRootDescriptorTable(2, srv_->GetGPUDescriptorHandle(textureSrvIndex_));
    cmdList->SetGraphicsRootConstantBufferView(3, cameraCB_->GetGPUVirtualAddress());

    cmdList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawInstanced(6, count, 0, 0);
}