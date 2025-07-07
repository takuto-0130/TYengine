#pragma once
#include "Camera.h"
#include "DirectXBasis.h"

class CubemapBasis
{
public:
    static CubemapBasis* GetInstance()
	{
		static CubemapBasis instance; // 静的ローカル変数（寿命がプログラム全体に渡る）
		return &instance;
	}
	~CubemapBasis() = default;
private:
	CubemapBasis() = default;
	CubemapBasis(CubemapBasis&) = delete;
	CubemapBasis& operator=(CubemapBasis&) = delete;
public:

    void Initialize(DirectXBasis* dxBasis);
    void DrawBegin();

    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState_.Get(); }

	void SetDefaultCamera(Camera* camera) { defaultCamera_ = camera; }
	Camera* GetDefaultCamera() const { return defaultCamera_; }

	DirectXBasis* GetDirectXBasis() const { return directXBasis_; }

private: // メンバ関数
	// ルートシグネチャの作成
	void CreateRootSignature();

	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();


private: // メンバ変数
	DirectXBasis* directXBasis_;
	Camera* defaultCamera_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

	D3D12_BLEND_DESC blendDesc_{};
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
};

