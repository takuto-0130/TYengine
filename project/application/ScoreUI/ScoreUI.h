#pragma once
#include "Sprite.h"

#include "Utils/Json/JsonManager.h"

#include <memory>
/// <summary>
/// スコア表示UIの管理クラス。
/// 桁ごとのスプライト管理やスコア加算時のアニメーション制御を行う。
/// </summary>
class ScoreUI
{
public:
	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	/// <summary>
	/// リザルト画面用の演出更新処理。
	/// </summary>
	/// <param name="currentTime">現在のアニメーション時間。</param>
	void UpdateResult(float currentTime);

	/// <summary>
	/// 表示するスコアを設定する。
	/// 現在値と異なる場合、カウントアップ演出が開始される。
	/// </summary>
	/// <param name="score">目標スコア。</param>
	void SetScore(int32_t score)
	{
		// 追加：現在の目標スコアと同じなら何もしない
		if (currentScore_ == score) return;

		scoreViewTimer_ = 0;
		prevScore_ = currentScore_;
		currentScore_ = score;
	}

	/// <summary>
	/// 仮対応：リザルト表示モードを設定する。
	/// </summary>
	void SetResult();


	std::vector<Sprite*>& GetSprite() { return setColliderSpr_; }

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void OffsetPos(const Vector2& pos);

	void ScoreViewSetting();

	void DebugJMApply();

private:
	enum RetryUISprites
	{
		THOUSANDS,
		HUNDREDS,
		TENS,
		ONES,
		SpriteNum
	};

	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	int32_t prevScore_ = 0;
	int32_t currentScore_ = 0;
	int32_t viewScore_ = 0;

	struct ScoreDisplay
	{
		std::array<int32_t, SpriteNum> num;
	};
	ScoreDisplay scoreDisplay_ = {};

	std::vector<Sprite*> setColliderSpr_;

	float scoreViewTimer_ = 0;
	float scoreViewTime_ = 0.0f;

	float deltaTime_ = 0.0f;

	jx::JsonManager* jm_;
};

