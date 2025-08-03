#pragma once
#include "CopyPass.h"

class OutlinePass
{
public:
	void Initialize(DirectXBasis* dx, SrvManager* srvMgr);
	void Update();
	void Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv);
	void SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle);

	void ImGuiUpdate();

public:
	// Setter
	void SetThreshold(float t) { outlineParam_->depthThreshold = t; }
	void SetOutlineWidth(float w) { outlineParam_->outlineWidth = w; }
	void SetTexelSize(const Vector2& texel) { outlineParam_->texelSize = texel; }
	void SetOutlineColor(const Vector3& color) { outlineParam_->outlineColor = color; }

	// Getter
	float GetThreshold() const { return outlineParam_->depthThreshold; }
	float GetOutlineWidth() const { return outlineParam_->outlineWidth; }
	Vector2 GetTexelSize() const { return outlineParam_->texelSize; }
	Vector3 GetOutlineColor() const { return outlineParam_->outlineColor; }

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

