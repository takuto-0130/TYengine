#pragma once
#include "Matrix4x4Func.h"
#include "mathFunc.h"
#include "WindowsApp.h"
#include "CameraShake.h"

class Camera
{
public: // メンバ関数
	Camera();

	// 更新
	void Update();

	// フォローカメラ
	// Update前での呼び出し推奨
	void FollowCamera(const Vector3& target);

	bool ShakeActive() { return shakeController_.IsActive(); }

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

	// 前フレームのカメラ位置（ワールド）
	Vector3 prevTranslate_ = {};

	// 今フレームのカメラ移動量（ワールド差分）
	Vector3 deltaTranslate_ = {};

	Vector3 followCameraOffsetPosition_ = Vector3(0.0f, 20.0f, -35.0f);

	Vector3 followCameraOffsetRotate_ = Vector3(0.51f, 0.0f, 0.0f);

	Vector3 shake_ = {};

	CameraShake shakeController_;

public: // メンバ関数
	void StartShake(const CameraShake::ShakeParams& params)
	{
		shakeController_.Start(params);
	}
    // ========================
    //        Setter
    // ========================

	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform_.translate = translate; }

	void SetOffsetRotate(const Vector3& rotate) { followCameraOffsetRotate_ = rotate; }
	void SetOffsetTranslate(const Vector3& translate) { followCameraOffsetPosition_ = translate; }

	void SetShake(const Vector3& shake) { shake_ = shake; }

	void SetFovY(float horizontalFOV) { horizontalFOV_ = horizontalFOV; }
	void SetAspectRate(float aspectRatio) { aspectRatio_ = aspectRatio; }
	void SetNearClip(float nearClip) { nearClip_ = nearClip; }
	void SetFarClip(float farClip) { farClip_ = farClip; }

	// ========================
	//        Getter
	// ========================

	const Vector3& GetTranslate() { return transform_.translate; }
	const Vector3& GetRotate() { return transform_.rotate; }
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return worldViewProjectionMatrix_; }
	const Vector3& GetDeltaTranslate() const { return deltaTranslate_; }


	const Vector3& GetShake() { return shake_; }

	Vector3 GetPosition() const { return transform_.translate; }
	Vector3 GetForward() const {
		// Z軸の向き（正面） = viewMatrix_ の 3列目
		return Normalize(Vector3{ viewMatrix_.m[0][2], viewMatrix_.m[1][2], viewMatrix_.m[2][2] });
	}

	Vector3 GetRight() const {
		// X軸の向き（右） = viewMatrix_ の 1列目
		return Normalize(Vector3{ viewMatrix_.m[0][0], viewMatrix_.m[1][0], viewMatrix_.m[2][0] });
	}

	Vector3 GetUp() const {
		// Y軸の向き（上） = viewMatrix_ の 2列目
		return Normalize(Vector3{ viewMatrix_.m[0][1], viewMatrix_.m[1][1], viewMatrix_.m[2][1] });
	}


	Vector3 GetLookForward() const {
		// Z軸の向き（正面） = worldMatrix_ の 3列目
		return Normalize(Vector3{ worldMatrix_.m[0][2], worldMatrix_.m[1][2], worldMatrix_.m[2][2] });
	}

	Vector3 GetLookRight() const {
		// X軸の向き（右） = worldMatrix_ の 1列目
		return Normalize(Vector3{ worldMatrix_.m[0][0], worldMatrix_.m[1][0], worldMatrix_.m[2][0] });
	}

	Vector3 GetLookUp() const {
		// Y軸の向き（上） = worldMatrix_ の 2列目
		return Normalize(Vector3{ worldMatrix_.m[0][1], worldMatrix_.m[1][1], worldMatrix_.m[2][1] });
	}
};
