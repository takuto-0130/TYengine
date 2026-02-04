#include "DoFPass.h"
#include "DirectXBasis.h"
#ifdef _DEBUG
#include "imgui.h"
#endif

namespace TYEngine
{
	namespace OffScreen
	{

		using namespace Core;
		using namespace Graphics;

		void DoFPass::Initialize(DirectXBasis* dx, SrvManager* srvMgr)
		{
			// シェーダ（CopyImage.VSとDepthOfField.PS）を使う
			copyPass_.Initialize(dx, srvMgr,
				L"Resources/Shaders/CopyImage.VS.hlsl",
				L"Resources/Shaders/DepthOfField.PS.hlsl");

			// DoF用パラメータバッファ確保
			dofParam_ = copyPass_.AddExtraConstantBuffer<DoFPassParam>(4);
		}

		void DoFPass::SetDepthSrv(D3D12_GPU_DESCRIPTOR_HANDLE handle)
		{
			copyPass_.SetDepthSrv(handle);
		}

		void DoFPass::ImGuiUpdate()
		{
#ifdef _DEBUG
			ImGui::SliderFloat("focusDepth", &dofParam_->focusDepth, 0.1f, 200.0f);
			ImGui::SliderFloat("blurScale", &dofParam_->blurScale, 1.0f, 30.0f);
#endif // _DEBUG
		}

		void DoFPass::Draw(ID3D12GraphicsCommandList* cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrv)
		{
			copyPass_.Draw(cmdList, inputSrv);
		}

		void DoFPass::Update()
		{
#ifdef _DEBUG
			ImGui::Begin("DoF");
			ImGuiUpdate();
			ImGui::End();
#endif
		}

	} // namespace OffScreen
} // namespace TYEngine
