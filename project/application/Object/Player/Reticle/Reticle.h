#pragma once
#include "ReticleCollider.h"
#include <memory>


class Camera;
class Input;
/// <summary>
/// プレイヤーの照準（レティクル）クラス。
/// 画面上の照準位置を管理し、ワールド空間上のレイを計算する。
/// </summary>
class Reticle
{
public:
	/// <summary>コンストラクタ。</summary>
	Reticle(TYEngine::CameraSystem::Camera* camera) : camera_(camera) {}
	~Reticle();

	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>更新処理。マウス入力等に基づいて位置を更新する。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

private:
	void ScreenToWorld();

public:
	/// <summary>ターゲットまでの距離を設定する。</summary>
	void SetFrameDistance(float distance) { frameDistance_ == 0.0f || frameDistance_ >= distance ? frameDistance_ = distance : frameDistance_; }
	/// <summary>現在のターゲット距離を取得する。</summary>
	float GetTargetDistance() const { return targetDistance_; }
	/// <summary>レティクルのレイを取得する。</summary>
	TYEngine::Utility::Ray GetRay() const { return collider_->GetRay(); }

	/// <summary>ワールド空間上のターゲット座標を取得する。</summary>
	TYEngine::Utility::Vector3 GetTarget() { return collider_->GetRay().origin + collider_->GetRay().diff * targetDistance_; }

private:
	TYEngine::CameraSystem::Camera* camera_ = nullptr;
	TYEngine::Framework::Input* input_ = nullptr;
	std::unique_ptr<ReticleCollider> collider_;
	float targetDistance_ = 0.0f;
	float frameDistance_ = 0.0f;
	const float defaultDistance_ = 50.0f; // 非調整項目のため const float
};

