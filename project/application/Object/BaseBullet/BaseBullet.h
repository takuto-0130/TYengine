#pragma once
#include "BaseObject.h"
/// <summary>
/// 弾オブジェクトの基底クラス。
/// 移動方向、速度、寿命（生存時間）、衝突判定の共通インターフェースを提供する。
/// </summary>
class BaseBullet :
    public TYEngine::Utility::BaseObject
{
public:
	BaseBullet()
	{
		colliderScale_ = 0.1f;
		scale_ = 0.05f;
		defaultSpeed_ = 1.0f;
		direction_ = { 0,0,1 };
		lifeTime_ = 5.0f;
	}
	/// <summary>
	/// 弾の進行方向（発射方向）を設定する。
	/// </summary>
	/// <param name="direction">正規化された方向ベクトル。</param>
	virtual void SetShotDirection(const TYEngine::Utility::Vector3& direction) { direction_ = direction; }

	/// <summary>
	/// 弾が消滅状態かどうかを取得する。
	/// </summary>
	/// <returns>消滅状態なら true。</returns>
	virtual bool IsDead() const { return isDead_; }

	/// <summary>
	/// 衝突時コールバック。
	/// デフォルトでは弾を消滅（死亡）させる。
	/// </summary>
	virtual void OnCollision() { isDead_ = true; }

	/// <summary>
	/// カメラへの参照を設定する（ビルボード処理などで使用）。
	/// </summary>
	/// <param name="camera">カメラインスタンス。</param>
	void SetCamera(TYEngine::CameraSystem::Camera* camera) { camera_ = camera; }

protected:
	/// <summary>死亡（消滅）フラグ。</summary>
	bool isDead_ = false;

	/// <summary>コライダーの半径スケール。</summary>
	float colliderScale_ = 0.0f;

	/// <summary>描画スケール。</summary>
	float scale_ = 0.0f;

	/// <summary>現在の速度ベクトル。</summary>
	TYEngine::Utility::Vector3 velocity_ = {};

	/// <summary>基本移動速度。</summary>
	float defaultSpeed_ = 0.0f;

	/// <summary>移動方向ベクトル。</summary>
	TYEngine::Utility::Vector3 direction_ = {};

	/// <summary>弾の生存寿命（秒）。0以下で消滅。</summary>
	float lifeTime_ = 0.0f;

	/// <summary>参照用カメラポインタ。</summary>
	TYEngine::CameraSystem::Camera* camera_ = nullptr;
};

