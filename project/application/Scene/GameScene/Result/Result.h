#pragma once
#include <memory>
#include "Sprite/Sprite.h"

#include "Utils/Json/JsonManager.h"

/// <summary>
/// リザルト画面の管理クラス。
/// スコア表示やタイマーによる演出制御を行う。
/// </summary>
class ResultClass
{
public:
	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	/// <summary>リザルト演出を開始する。</summary>
	void Start();

	/// <summary>演出中の動作処理。</summary>
	void Move();

	/// <summary>状態をリセットする。</summary>
	void Reset();

	/// <summary>
	/// コライダーに設定するスプライトのリストを取得する。
	/// </summary>
	std::vector<Sprite*>& GetSprite() { return setColliderSpr_; }

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void DebugJMApply();

private:
	enum ResultUISprites
	{
		BLACK,
		RESULT,
		SCORE,
		SPACE,
		SpriteNum
	};

	/// <summary>スプライト配列。</summary>
	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	/// <summary>コライダー設定用スプライトリスト。</summary>
	std::vector<Sprite*> setColliderSpr_;

	/// <summary>演出タイマー。</summary>
	float timer_ = 0.0f;

	/// <summary>演出最大時間。</summary>
	float maxTime_ = 0.0f;

	/// <summary>JSONマネージャ。</summary>
	jx::JsonManager* jm_;
};

