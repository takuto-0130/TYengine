#include "CopyImage.h"
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "RenderTexture.h"

namespace TYEngine
{
	namespace OffScreen
	{

		using namespace Core;
		using namespace Graphics;

		void CopyImageEffect::Initialize(DirectXBasis* dx, SrvManager* srv)
		{
			dx_ = dx;
			// シェーダ（CopyImage.VSとCopyImage.PS）を使う（単純コピー）
			copyPass_.Initialize(dx_, srv, L"Resources/Shaders/CopyImage.VS.hlsl", L"Resources/Shaders/CopyImage.PS.hlsl");
		}

		void CopyImageEffect::Update()
		{}

		void CopyImageEffect::ImGuiUpdate()
		{}

		void CopyImageEffect::Apply(RenderTexture* input)
		{
			copyPass_.Draw(dx_->GetCommandList(), input->GetGPUHandle());
		}

	} // namespace OffScreen
} // namespace TYEngine
