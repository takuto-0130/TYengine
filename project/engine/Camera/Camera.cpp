#include "Camera.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


Camera::Camera()
	: transform_({ {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} })
	, horizontalFOV_(0.45f)
	, aspectRatio_(float(WindowsApp::kClientWidth) / float(WindowsApp::kClientHeight))
	, nearClip_(2.0f)
	, farClip_(500.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_))
	, worldViewProjectionMatrix_(viewMatrix_ * projectionMatrix_)
{
	prevTranslate_ = transform_.translate;
	deltaTranslate_ = Vector3{};
}

void Camera::Update()
{
#ifdef _DEBUG
	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &transform_.scale.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

	// --- カメラ移動量の算出（ワールド基準） ---
	deltaTranslate_ = transform_.translate - prevTranslate_;
	prevTranslate_ = transform_.translate;

	// --- シェイク ---
	shakeController_.Update(Timer::GetInstance()->GetDeltaTime());
	shake_ = shakeController_.GetOffset();
	
	// --- 行列更新（※差分計算には shake を含めない） ---
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate + shake_);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(horizontalFOV_, aspectRatio_, nearClip_, farClip_);
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