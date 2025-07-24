#include "OutlinePass.h"
#include "DirectXBasis.h"

void OutlinePass::Initialize(DirectXBasis* dx, SrvManager* srvMgr) {
    copyPass_.Initialize(dx, srvMgr, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/DepthBasedOutline.PS.hlsl");
    outlineParam_ = copyPass_.AddExtraConstantBuffer<OutlinePassParam>(4);
}

void OutlinePass::SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle) {
    copyPass_.SetDepthSrv(handle); // register(t1) にバインド
}

void OutlinePass::Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv) {
    copyPass_.Draw(cmdList, inputSrv);
}
