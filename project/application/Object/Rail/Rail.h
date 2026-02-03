#pragma once
#include "BaseObject.h"

/// <summary>
/// レールオブジェクトクラス。
/// キャラクターが移動する軌跡やパスとして機能するオブジェクト。
/// </summary>
class Rail : public BaseObject
{
public:

	~Rail();

	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>更新処理。</summary>
	void Update() override;

	/// <summary>描画処理。</summary>
	void Draw() override;

	/// <summary>回転を設定する。</summary>
	void SetRotate(const Vector3& rotation) { worldTransform_.SetRotate(rotation); }
	/// <summary>位置を設定する。</summary>
	void SetTranslation(const Vector3& translation) { worldTransform_.SetTranslation(translation); }
};
