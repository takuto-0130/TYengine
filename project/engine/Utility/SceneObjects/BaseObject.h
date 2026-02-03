#pragma once
#include "Object3d.h"
#include "WorldTransform.h"

/// <summary>
/// オブジェクトの基底クラス。
/// 3Dモデル、ワールド変換行列を保持し、基本的な更新・描画インターフェースを提供する。
/// </summary>
class BaseObject
{
public:
	/// <summary>デストラクタ。</summary>
	virtual ~BaseObject() {};

	/// <summary>初期化処理。</summary>
	virtual void Init() = 0;

	/// <summary>更新処理。</summary>
	virtual void Update() = 0;

	/// <summary>描画処理。</summary>
	virtual void Draw() = 0;

	/// <summary>
	/// ワールド座標を取得する。
	/// </summary>
	/// <returns>ワールド座標。</returns>
	virtual Vector3 GetWorldPosition()
	{
		Vector3 worldPos;
		worldPos.x = worldTransform_.GetMatWorld().m[3][0];
		worldPos.y = worldTransform_.GetMatWorld().m[3][1];
		worldPos.z = worldTransform_.GetMatWorld().m[3][2];
		return worldPos;
	}

	/// <summary>
	/// ローカルスケールを設定する。
	/// </summary>
	/// <param name="scale">スケール値。</param>
	virtual void SetScale(const Vector3& scale) { worldTransform_.SetScale(scale); }

	/// <summary>
	/// ローカル回転を設定する。
	/// </summary>
	/// <param name="rotate">回転角（ラジアン）。</param>
	virtual void SetRotate(const Vector3& rotate) { worldTransform_.SetRotate(rotate); }

	/// <summary>
	/// ローカル座標を設定する。
	/// </summary>
	/// <param name="pos">位置座標。</param>
	virtual void SetTranslation(const Vector3& pos) { worldTransform_.SetTranslation(pos); }

protected:
	// 3Dオブジェクト
	std::unique_ptr<Object3d> obj_;

	// ワールド行列
	WorldTransform worldTransform_;
};

