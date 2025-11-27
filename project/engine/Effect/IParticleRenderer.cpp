#include "IParticleRenderer.h"
#include "Timer.h"
#include <numbers>
#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG


void IParticleRenderer::Initialize(DirectXBasis* dx, SrvManager* srv, Camera* cam) {
    dxBasis_ = dx;
    srvManager_ = srv;
    camera_ = cam;

    emitter_.transform.scale = { 1,1,1 };
    emitter_.count = 5;
    emitter_.frequency = 0.5f;

    CreateResources();
    emitterBuffer_ = dxBasis_->CreateBufferResource(
        sizeof(EmitterParam),
        D3D12_HEAP_TYPE_UPLOAD,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );


    CreateRootSignature();
    LoadShader();
    CreatePipelineState();
}

void IParticleRenderer::Update() {
    std::mt19937 random(seedGene_());

    Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
    Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorldMatrix());
    billboardMatrix.m[3][0] = 0.0f;
    billboardMatrix.m[3][1] = 0.0f;
    billboardMatrix.m[3][2] = 0.0f;

    numInstance_ = 0;

    kDeltaTime = Timer::GetInstance()->GetDeltaTime();

    emitter_.frequencyTime += kDeltaTime;
    if (!useTrigger_ &&  emitter_.frequencyTime >= emitter_.frequency) {
        particles_.splice(particles_.end(), Emit(random));
        emitter_.frequencyTime -= emitter_.frequency;
    }

    for (auto it = particles_.begin(); it != particles_.end();) {
        it->currentTime += kDeltaTime;
        it->transform.translate += it->velocity * kDeltaTime;

        if (it->currentTime >= it->lifeTime) {
            it = particles_.erase(it);
            continue;
        }

        if (numInstance_ < kMaxInstance) {
            Matrix4x4 world = MakeAffineMatrix(it->transform.scale, it->transform.rotate, it->transform.translate);
            if (useBillboard_) {
                world = MakeScaleMatrix(it->transform.scale) 
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

void IParticleRenderer::Draw() {
    if (numInstance_ == 0) return;

    auto cmd = dxBasis_->GetCommandList();

    cmd->SetGraphicsRootSignature(rootSignature_.Get());
    cmd->SetPipelineState(pipelineState_.Get());

    cmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmd->IASetVertexBuffers(0, 1, &vertexBufferView_);

    srvManager_->SetGraphicsRootDescriptorTable(cmd, 1, srvIndex_);
    srvManager_->SetGraphicsRootDescriptorTable(cmd, 2, textureIndex_);
    cmd->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    cmd->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());

    cmd->DrawInstanced(vertexCount_, numInstance_, 0, 0);
}

void IParticleRenderer::TriggerEmit()
{
    if(useTrigger_)
    {
        std::mt19937 random(seedGene_());
        particles_.splice(particles_.end(), Emit(random));
    }
}

IParticleRenderer::ParticleP IParticleRenderer::MakeNewParticle(std::mt19937& random, const ParticleEmitter& emitter) {
    ParticleP parti;
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    parti.transform.scale = { 1.f,1.f,1.f };
    parti.transform.rotate = { 0.f,0.f,0.f };
    parti.transform.translate = Vector3{ dist(random),dist(random),dist(random) } + emitter.transform.translate;
    parti.velocity = { dist(random),dist(random),dist(random) };

    std::uniform_real_distribution<float> colorDist(0.0f, 1.0f);
    parti.color = { colorDist(random), colorDist(random), colorDist(random), 1.0f };

    std::uniform_real_distribution<float> timeDist(1.0f, 3.0f);
    parti.lifeTime = timeDist(random);
    parti.currentTime = 0.0f;

    return parti;
}

std::list<IParticleRenderer::ParticleP> IParticleRenderer::Emit(std::mt19937& random) {
    std::list<ParticleP> result;
    for (uint32_t i = 0; i < emitter_.count; ++i) {
        result.push_back(MakeNewParticle(random, emitter_));
    }
    return result;
}

void IParticleRenderer::CreateRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC desc = {};
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // 4 params:
    // 0: CBV b0 (e.g. material/transform) - make visible where needed
    // 1: SRV table t0 (particle StructuredBuffer) - VERTEX
    // 2: SRV table t1 (texture) - PIXEL
    // 3: CBV b1 (camera) - VERTEX (required by VS)
    D3D12_ROOT_PARAMETER params[4] = {};

    // param 0: CBV b0
    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    params[0].Descriptor.ShaderRegister = 0;
    params[0].Descriptor.RegisterSpace = 0;
    // Use ALL visibility if the CBV is accessed from VS and/or PS.
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    // param 1: SRV table t0 (particle buffer) - used in VS
    D3D12_DESCRIPTOR_RANGE range1 = {};
    range1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range1.NumDescriptors = 1;
    range1.BaseShaderRegister = 0; // t0
    range1.RegisterSpace = 0;
    range1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[1].DescriptorTable.NumDescriptorRanges = 1;
    params[1].DescriptorTable.pDescriptorRanges = &range1;
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    // param 2: SRV table t1 (texture) - used in PS
    D3D12_DESCRIPTOR_RANGE range2 = {};
    range2.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    range2.NumDescriptors = 1;
    range2.BaseShaderRegister = 1; // t1
    range2.RegisterSpace = 0;
    range2.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    params[2].DescriptorTable.NumDescriptorRanges = 1;
    params[2].DescriptorTable.pDescriptorRanges = &range2;
    params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // param 3: CBV b1 (Camera) - required by VS
    params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    params[3].Descriptor.ShaderRegister = 1; // b1
    params[3].Descriptor.RegisterSpace = 0;
    params[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // ← ここがポイント

    desc.NumParameters = _countof(params);
    desc.pParameters = params;

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    desc.NumStaticSamplers = 1;
    desc.pStaticSamplers = &sampler;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        assert(false);
    }
    dxBasis_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
}


void IParticleRenderer::LoadShader() {
    vsBlob_ = dxBasis_->CompileShader(L"Resources/Shaders/Particle.VS.hlsl", L"vs_6_0");
    psBlob_ = dxBasis_->CompileShader(L"Resources/Shaders/Particle.PS.hlsl", L"ps_6_0");
    assert(vsBlob_ && psBlob_);
}

void IParticleRenderer::CreatePipelineState() {
    D3D12_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,      0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc{};
    desc.pRootSignature = rootSignature_.Get();
    desc.InputLayout = { layout, _countof(layout) };
    desc.VS = { vsBlob_->GetBufferPointer(), vsBlob_->GetBufferSize() };
    desc.PS = { psBlob_->GetBufferPointer(), psBlob_->GetBufferSize() };
    desc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    desc.BlendState.RenderTarget[0].BlendEnable = TRUE;
    desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
    desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    desc.DepthStencilState.DepthEnable = TRUE;
    desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.SampleDesc.Count = 1;
    desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    HRESULT hr = dxBasis_->GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(hr));
}

void IParticleRenderer::CreateComputeRootSignatureAndPSO()
{ // RootParam: UAV(u0), CBV(b0) 
    D3D12_ROOT_PARAMETER params[5] = {}; 
    // u0 = gParticles UAV
    D3D12_DESCRIPTOR_RANGE range0 = { D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0 }; 
    params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; 
    params[0].DescriptorTable.NumDescriptorRanges = 1; 
    params[0].DescriptorTable.pDescriptorRanges = &range0; 
    params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 
    // u1 = gEmitterState UAV 
    D3D12_DESCRIPTOR_RANGE range1 = { D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1 }; 
    params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; 
    params[1].DescriptorTable.NumDescriptorRanges = 1; 
    params[1].DescriptorTable.pDescriptorRanges = &range1; 
    params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 
    // b0 = CSParams 
    params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; 
    params[2].Descriptor.ShaderRegister = 0; 
    params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 
    // b1 = Camera 
    params[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; 
    params[3].Descriptor.ShaderRegister = 1; 
    params[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 
    // b2 = EmitterParam 
    params[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; 
    params[4].Descriptor.ShaderRegister = 2; 
    params[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; 
    D3D12_ROOT_SIGNATURE_DESC desc{}; 
    desc.NumParameters = _countof(params); 
    desc.pParameters = params; 
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE; 
    Microsoft::WRL::ComPtr<ID3DBlob> sigBlob, errBlob; 
    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob); 
    assert(SUCCEEDED(hr)); dxBasis_->GetDevice()->CreateRootSignature(
        0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), 
        IID_PPV_ARGS(&computeRootSignature_)); 
    // PSO 
    Microsoft::WRL::ComPtr<IDxcBlob> csBlob = dxBasis_->CompileShader(L"Resources/Shaders/Particle.CS.hlsl", L"cs_6_0"); 
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{}; 
    psoDesc.pRootSignature = computeRootSignature_.Get(); 
    psoDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() }; 
    hr = dxBasis_->GetDevice()->CreateComputePipelineState(
        &psoDesc, IID_PPV_ARGS(&computePipelineState_)); assert(SUCCEEDED(hr)); 
}
