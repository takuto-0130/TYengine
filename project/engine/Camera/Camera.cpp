#include "Camera.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


Camera::Camera()
	: transform_({ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} })
	, horizontalFOV_(0.45f)
	, aspectRatio_(float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHieght))
	, nearClip_(0.1f)
	, farClip_(500.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_))
	, worldViewProjectionMatrix_(viewMatrix_ * projectionMatrix_)
{}

void Camera::Update()
{
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

	shakeController_.Update(Timer::GetInstance()->GetDeltaTime());
	shake_ = shakeController_.GetOffset();

	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate + shake_);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_);
	worldViewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void Camera::FollowCamera(const Vector3& target)
{
	// カメラの位置を対象の後方に設定
	transform_.rotate = followCameraOffsetRotare_;
	transform_.translate = target + followCameraOffsetPosition_;
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
}