#include "Camera.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


namespace TYEngine
{
	namespace CameraSystem
	{

		using namespace TYEngine::Utility;
		using namespace TYEngine::Core;

		Camera::Camera()
			: transform_({ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} })
			, horizontalFOV_(0.45f)
			, aspectRatio_(float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHeight))
			, nearClip_(2.0f)
			, farClip_(500.0f)
			, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
			, viewMatrix_(Inverse(worldMatrix_))
			, projectionMatrix_(MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_))
			, worldViewProjectionMatrix_(viewMatrix_* projectionMatrix_)
		{
			prevTranslate_ = transform_.translate;
			deltaTranslate_ = Vector3{};
		}

		void Camera::Update()
		{
#ifdef _DEBUG
			// デバッグ用UI：カメラパラメータの調整
			ImGui::Begin("camera");
			ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
			ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
			ImGui::DragFloat3("scale", &transform_.scale.x, 0.1f);
			ImGui::End();
#endif // _DEBUG

			// --- カメラ移動量の算出（ワールド基準） ---
			deltaTranslate_ = transform_.translate - prevTranslate_;
			prevTranslate_ = transform_.translate;

			// --- シェイク（振動）効果の更新 ---
			shakeController_.Update(Timer::GetInstance()->GetDeltaTime());
			shake_ = shakeController_.GetOffset();

			// --- 行列更新（※視点計算には shake を含める） ---
			// ワールド行列（移動＋振動）
			worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate + shake_);
			// ビュー行列（ワールド行列の逆行列）
			viewMatrix_ = Inverse(worldMatrix_);
			// プロジェクション行列（透視投影）
			projectionMatrix_ = MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_);
			// 合成行列（WVP）
			worldViewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
		}

		void Camera::FollowCamera(const Vector3& target)
		{
			// カメラの位置を対象の後方に設定
			transform_.rotate = followCameraOffsetRotate_;
			transform_.translate = target + followCameraOffsetPosition_;
			worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
			viewMatrix_ = Inverse(worldMatrix_);
		}

		bool Camera::WorldToNDC(const Utility::Vector3& worldPos, Utility::Vector2& outNDC) const
		{
			// ビュー×プロジェクション行列を使用
			Utility::Matrix4x4 vp = worldViewProjectionMatrix_;

			// W成分（カメラからの深度）を計算
			float w = worldPos.x * vp.m[0][3] + worldPos.y * vp.m[1][3] + worldPos.z * vp.m[2][3] + vp.m[3][3];

			// カメラの後ろにいる場合は除外
			if (w < 0.1f) return false;

			// X, Y成分を計算してWで割る（正規化デバイス座標へ）
			float x = worldPos.x * vp.m[0][0] + worldPos.y * vp.m[1][0] + worldPos.z * vp.m[2][0] + vp.m[3][0];
			float y = worldPos.x * vp.m[0][1] + worldPos.y * vp.m[1][1] + worldPos.z * vp.m[2][1] + vp.m[3][1];

			outNDC.x = x / w;
			outNDC.y = y / w;

			// NDC範囲（-1.0 ~ 1.0）内に収まっているか判定
			return (outNDC.x >= -1.0f && outNDC.x <= 1.0f &&
				outNDC.y >= -1.0f && outNDC.y <= 1.0f);
		}

	} // namespace Camera
} // namespace TYEngine
