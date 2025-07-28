#pragma once
#include "CopyPass.h"

class DoFPass
{
public:
    void Initialize(DirectXBasis* dx, SrvManager* srvMgr);
    void Update();
    void ImGuiUpdate();
    void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv);
    void SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle);

    // Setter / Getter
    void SetFocusDepth(float depth) { dofParam_->focusDepth = depth; }
    void SetBlurScale(float scale) { dofParam_->blurScale = scale; }
    void SetNearFar(float nearZ, float farZ) { dofParam_->nearZ = nearZ; dofParam_->farZ = farZ; }

private:
    struct DoFPassParam 
    {
        float nearZ = 0.1f;
        float farZ = 1000.0f;
        float focusDepth = 4.0f;
        float blurScale = 1.0f;   // COC → ブラー半径変換係数
    };

    std::shared_ptr<DoFPassParam> dofParam_;
    CopyPass copyPass_;
};
