#pragma once
#include "Matrix4x4Func.h"
#include "mathFunc.h"
#include "WindowsApp.h"
#include "CameraShake.h"

namespace TYEngine
{
	namespace CameraSystem
	{

		/// <summary>
		/// ゲーム内カメラを管理するクラス。
		/// ビュー行列・プロジェクション行列の計算、カメラシェイク、追従機能などを提供する。
		/// </summary>
		class Camera
		{
		public: // メンバ関数
			Camera();

			/// <summary>
			/// 毎フレームの更新処理。
			/// シェイクの更新、行列の再計算を行う。
			/// </summary>
			void Update();

			/// <summary>
			/// 指定したターゲット（追従対象）の位置に合わせてカメラ位置を更新する。
			/// Update の前に呼び出すことを推奨。
			/// </summary>
			/// <param name="target">追従対象のワールド座標。</param>
			void FollowCamera(const Utility::Vector3& target);

			/// <summary>
			/// 現在カメラシェイクが有効かどうかを判定する。
			/// </summary>
			/// <returns>シェイク中なら true。</returns>
			bool ShakeActive() { return shakeController_.IsActive(); }

		private: // メンバ変数
			/// <summary>カメラのトランスフォーム（位置・回転・スケール）。</summary>
			Utility::Transform transform_;
			/// <summary>ワールド行列。</summary>
			Utility::Matrix4x4 worldMatrix_;
			/// <summary>ビュー行列。</summary>
			Utility::Matrix4x4 viewMatrix_;
			/// <summary>プロジェクション行列。</summary>
			Utility::Matrix4x4 projectionMatrix_;
			/// <summary>ビュープロジェクション行列。</summary>
			Utility::Matrix4x4 worldViewProjectionMatrix_;

			/// <summary>水平視野角（FOV）。</summary>
			float horizontalFOV_;
			/// <summary>アスペクト比。</summary>
			float aspectRatio_;
			/// <summary>ニアクリップ平面までの距離。</summary>
			float nearClip_;
			/// <summary>ファークリップ平面までの距離。</summary>
			float farClip_;

			/// <summary>前フレームのカメラ位置（ワールド座標）。</summary>
			Utility::Vector3 prevTranslate_ = {};

			/// <summary>今フレームのカメラ移動量（ワールド座標差分）。</summary>
			Utility::Vector3 deltaTranslate_ = {};

			/// <summary>追従カメラ時のターゲットからのオフセット位置。</summary>
			Utility::Vector3 followCameraOffsetPosition_ = Utility::Vector3(0.0f, 20.0f, -35.0f);

			/// <summary>追従カメラ時のオフセット回転。</summary>
			Utility::Vector3 followCameraOffsetRotate_ = Utility::Vector3(0.51f, 0.0f, 0.0f);

			/// <summary>現在のシェイクによるオフセット量。</summary>
			Utility::Vector3 shake_ = {};

			/// <summary>カメラシェイク制御クラス。</summary>
			CameraShake shakeController_;

		public: // メンバ関数
			/// <summary>
			/// カメラシェイクを開始する。
			/// </summary>
			/// <param name="params">シェイクのパラメータ（強度・時間など）。</param>
			void StartShake(const CameraShake::ShakeParams& params)
			{
				shakeController_.Start(params);
			}
			// ========================
			//        Setter
			// ========================

			void SetRotate(const Utility::Vector3& rotate) { transform_.rotate = rotate; }
			void SetTranslate(const Utility::Vector3& translate) { transform_.translate = translate; }

			void SetOffsetRotate(const Utility::Vector3& rotate) { followCameraOffsetRotate_ = rotate; }
			void SetOffsetTranslate(const Utility::Vector3& translate) { followCameraOffsetPosition_ = translate; }

			void SetShake(const Utility::Vector3& shake) { shake_ = shake; }

			void SetFovY(float horizontalFOV) { horizontalFOV_ = horizontalFOV; }
			void SetAspectRate(float aspectRatio) { aspectRatio_ = aspectRatio; }
			void SetNearClip(float nearClip) { nearClip_ = nearClip; }
			void SetFarClip(float farClip) { farClip_ = farClip; }

			// ========================
			//        Getter
			// ========================

			const Utility::Vector3& GetTranslate() { return transform_.translate; }
			const Utility::Vector3& GetRotate() { return transform_.rotate; }
			const Utility::Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
			const Utility::Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
			const Utility::Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
			const Utility::Matrix4x4& GetViewProjectionMatrix() const { return worldViewProjectionMatrix_; }
			const Utility::Vector3& GetDeltaTranslate() const { return deltaTranslate_; }


			const Utility::Vector3& GetShake() { return shake_; }

			Utility::Vector3 GetPosition() const { return transform_.translate; }
			Utility::Vector3 GetForward() const
			{
				// Z軸の向き（正面） = viewMatrix_ の 3列目
				return Utility::Normalize(Utility::Vector3{ viewMatrix_.m[0][2], viewMatrix_.m[1][2], viewMatrix_.m[2][2] });
			}

			Utility::Vector3 GetRight() const
			{
				// X軸の向き（右） = viewMatrix_ の 1列目
				return Utility::Normalize(Utility::Vector3{ viewMatrix_.m[0][0], viewMatrix_.m[1][0], viewMatrix_.m[2][0] });
			}

			Utility::Vector3 GetUp() const
			{
				// Y軸の向き（上） = viewMatrix_ の 2列目
				return Utility::Normalize(Utility::Vector3{ viewMatrix_.m[0][1], viewMatrix_.m[1][1], viewMatrix_.m[2][1] });
			}


			Utility::Vector3 GetLookForward() const
			{
				// Z軸の向き（正面） = worldMatrix_ の 3列目
				return Utility::Normalize(Utility::Vector3{ worldMatrix_.m[0][2], worldMatrix_.m[1][2], worldMatrix_.m[2][2] });
			}

			Utility::Vector3 GetLookRight() const
			{
				// X軸の向き（右） = worldMatrix_ の 1列目
				return Utility::Normalize(Utility::Vector3{ worldMatrix_.m[0][0], worldMatrix_.m[1][0], worldMatrix_.m[2][0] });
			}

			Utility::Vector3 GetLookUp() const
			{
				// Y軸の向き（上） = worldMatrix_ の 2列目
				return Utility::Normalize(Utility::Vector3{ worldMatrix_.m[0][1], worldMatrix_.m[1][1], worldMatrix_.m[2][1] });
			}
		};

	} // namespace Camera
} // namespace TYEngine
