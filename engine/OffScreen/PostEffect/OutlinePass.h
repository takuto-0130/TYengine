#pragma once
#include "CopyPass.h"

class OutlinePass
{
public:
	void Initialize(DirectXBasis* dx, SrvManager* srvMgr);
	void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv);
	void SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle);

private:
	struct OutlinePassParam {
		Vector2 texelSize = { 1.0f / 1280.0f, 1.0f / 720.0f }; // レンダリングターゲットの解像度
		float depthThreshold = 0.001f;
		float pad0;
		Vector3 outlineColor = { 0.0f, 0.0f, 0.0f };
		float outlineWidth = 1.0f;
		float pad1[3] = {};
	};

	std::shared_ptr<OutlinePassParam> outlineParam_;

	CopyPass copyPass_;
};

