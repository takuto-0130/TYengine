#include "RadialBlurEffect.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

namespace TYEngine
{
	namespace OffScreen
	{

		using namespace Core;
		using namespace Graphics;

		void RadialBlurEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
		{
			dx_ = dx;
			// シェーダ読み込み
			copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/RadialBlur.PS.hlsl");
			// 定数バッファ確保
			param_ = copyPass_.AddExtraConstantBuffer<RadialBlurParam>(4);
		}

		void RadialBlurEffect::Update()
		{
#ifdef _DEBUG
			ImGui::Begin("RadialBlur");
			ImGuiUpdate();
			ImGui::End();
#endif // _DEBUG
		}

		void RadialBlurEffect::ImGuiUpdate()
		{
#ifdef _DEBUG
			if (ImGui::TreeNode("RadialBlur"))
			{
				ImGui::SliderFloat2("Center", &param_->kCenter.x, 0.0f, 1.0f);
				ImGui::SliderFloat("BlurWidth", &param_->kBlurWidth, 0.0f, 1.0f);
				ImGui::SliderInt("Sample", &param_->kNumSamples, 1, 50);
				ImGui::TreePop();
			}
#endif // _DEBUG
		}

		void RadialBlurEffect::Apply(RenderTexture* input)
		{
			// CopyPassを使って描画
			copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
		}

	} // namespace OffScreen
} // namespace TYEngine
