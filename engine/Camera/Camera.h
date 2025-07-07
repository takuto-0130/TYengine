#pragma once
#include "Matrix4x4Func.h"
#include "mathFunc.h"
#include "WindowsApp.h"
class Camera
{
public: // メンバ関数
	Camera();

	// 更新
	void Update();

	// フォローカメラ
	void FollowCamera(const Vector3& target);

private: // メンバ変数
	Transform transform_;
	Matrix4x4 worldMatrix_;
	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 worldViewProjectionMatrix_;
	float horizontalFOV_;
	float aspectRatio_;
	// ニアクリップ距離
	float nearClip_;
	// ファークリップ距離
	float farClip_;

	Vector3 followCameraOffsetPosition_ = Vector3(0.0f, 20.0f, -35.0f);

	Vector3 followCameraOffsetRotare_ = Vector3(0.51f, 0.0f, 0.0f);

public: // メンバ関数
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	const Vector3& GetRotate() { return transform_.rotate; }
	
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }
	const Vector3& GetTranslate() { return transform_.translate; }

	void SetOffsetRotate(const Vector3& rotate) { followCameraOffsetRotare_ = rotate; }

	void SetOffsetTranslate(const Vector3& translate) { followCameraOffsetPosition_ = translate; }

	void SetFovY(const float& horizontalFOV) { horizontalFOV_ = horizontalFOV; }
	void SetAspectRate(const float& aspectRatio) { aspectRatio_ = aspectRatio; }
	void SetNearClip(const float& nearClip) { nearClip_ = nearClip; }
	void SetFarClip(const float& farClip) { farClip_ = farClip; }

	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return worldViewProjectionMatrix_; }

	Vector3 GetPosition() const { return transform_.translate; }
	Vector3 GetForward() const {
		// Z軸の向き（正面） = viewMatrix_ の 3列目
		return Normalize({ viewMatrix_.m[0][2], viewMatrix_.m[1][2], viewMatrix_.m[2][2] });
	}

	Vector3 GetRight() const {
		// X軸の向き（右） = viewMatrix_ の 1列目
		return Normalize({ viewMatrix_.m[0][0], viewMatrix_.m[1][0], viewMatrix_.m[2][0] });
	}

	Vector3 GetUp() const {
		// Y軸の向き（上） = viewMatrix_ の 2列目
		return Normalize({ viewMatrix_.m[0][1], viewMatrix_.m[1][1], viewMatrix_.m[2][1] });
	}
};
