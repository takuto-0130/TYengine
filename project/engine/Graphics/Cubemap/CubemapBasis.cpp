#include "CubemapBasis.h"
#include "TextureManager.h"
#include "Logger.h"

namespace TYEngine
{
	namespace Graphics
	{

		using namespace Debugger;
		using namespace Core;

		void CubemapBasis::Initialize(DirectXBasis* directXBasis)
		{
			directXBasis_ = directXBasis;

			CreateRootSignature();
			CreateGraphicsPipeline();

		}

		void CubemapBasis::DrawBegin()
		{
			// スカイボックスパイプライン設定
			directXBasis_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
			directXBasis_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
			directXBasis_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// ディスクリプタヒープセット (TextureManager経由)
			ID3D12DescriptorHeap* heaps[] =
			{
			TextureManager::GetInstance()->GetSrvManager()->GetHeap()
			};
			directXBasis_->GetCommandList()->SetDescriptorHeaps(_countof(heaps), heaps);
		}

		void CubemapBasis::CreateRootSignature()
		{
			//RootSignatureを生成する
			D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
			descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

			//RootParameter作成。複数設定できるので配列。
			D3D12_ROOT_PARAMETER rootParameters[4] = {};
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[0].Descriptor.ShaderRegister = 0; //b0の0と一致　b11と紐づけたいなら11となる

			rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			rootParameters[1].Descriptor.ShaderRegister = 1;

			D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
			descriptorRange[0].BaseShaderRegister = 0;
			descriptorRange[0].NumDescriptors = 1;
			descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
			rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

			rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
			rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParameters[3].Descriptor.ShaderRegister = 2;

			descriptionRootSignature.pParameters = rootParameters;
			descriptionRootSignature.NumParameters = _countof(rootParameters);

			//Samplerの設定
			D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
			staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
			staticSamplers[0].ShaderRegister = 0;
			staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			descriptionRootSignature.pStaticSamplers = staticSamplers;
			descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

			HRESULT hr = S_FALSE;

			//シリアライズしてバイナリにする
			hr = D3D12SerializeRootSignature(&descriptionRootSignature,
				D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
			if (FAILED(hr))
			{
				Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
				assert(false);
			}
			//バイナリを元に生成
			hr = directXBasis_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
				signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
			assert(SUCCEEDED(hr));
		}

		void CubemapBasis::CreateGraphicsPipeline()
		{
			//InputLayout
			D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
			inputElementDescs[0].SemanticName = "POSITION";
			inputElementDescs[0].SemanticIndex = 0;
			inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
			inputLayoutDesc.pInputElementDescs = inputElementDescs;
			inputLayoutDesc.NumElements = _countof(inputElementDescs);

			//BlendState
			//すべての色要素を書き込む
			blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			blendDesc_.RenderTarget[0].BlendEnable = TRUE;
			blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

			//RasterizerState
			D3D12_RASTERIZER_DESC rasterizerDesc{};
			rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
			rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


			//shaderCompile
			vertexShaderBlob_ = directXBasis_->CompileShader(L"Resources/Shaders/Skybox.VS.hlsl",
				L"vs_6_0");
			assert(vertexShaderBlob_ != nullptr);

			pixelShaderBlob_ = directXBasis_->CompileShader(L"Resources/Shaders/Skybox.PS.hlsl",
				L"ps_6_0");
			assert(pixelShaderBlob_ != nullptr);

			//DepthStencilStateの設定
			D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
			//Depthの機能を有効化する
			depthStencilDesc.DepthEnable = true;
			//書き込みしない
			depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			//比較関数はLessEqual。つまり、近ければ描画される
			depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


			//PSO作成
			D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
			graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
			graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
			graphicsPipelineStateDesc.BlendState = blendDesc_;
			graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
			graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),
			vertexShaderBlob_->GetBufferSize() };
			graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),
			pixelShaderBlob_->GetBufferSize() };
			//DepthStencilの設定
			graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
			graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			//書き込むRTVの情報
			graphicsPipelineStateDesc.NumRenderTargets = 1;
			graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			//形状
			graphicsPipelineStateDesc.PrimitiveTopologyType =
				D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			//どのように画面に打ち込むかの設定
			graphicsPipelineStateDesc.SampleDesc.Count = 1;
			graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
			//実際に生成
			HRESULT hr = S_FALSE;
			hr = directXBasis_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
				IID_PPV_ARGS(&graphicsPipelineState_));
			assert(SUCCEEDED(hr));
		}

	} // namespace Graphics
} // namespace TYEngine
