#pragma once
#include "ParticleSystem.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include <unordered_map>
#include <string>
#include <wrl.h>
#include <d3d12.h>

class ParticleManager {
public:
    void Initialize(DirectXBasis* dx, SrvManager* srv);
    void Update(float deltaTime);
    void Draw();

    ParticleSystem* CreateSystem(const std::string& name);
    void RemoveSystem(const std::string& name);

    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); }

private:
    void CreateRootSignature();
    void CreatePipelineState();

    DirectXBasis* dx_ = nullptr;
    SrvManager* srv_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

    std::unordered_map<std::string, std::unique_ptr<ParticleSystem>> systems_;
};

