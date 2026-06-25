#pragma once
#include "Sprite.h"

#include "BaseUI.h"

#include <memory>

/// <summary>
/// スコア表示UIの管理クラス。
/// 桁ごとのスプライト管理やスコア加算時のアニメーション制御を行う。
/// </summary>
class ScoreUI : public BaseUI
{
public:
	/// <summary>初期化処理。</summary>
	void Init() override;

	/// <summary>更新処理。</summary>
	void Update() override;

	/// <summary>描画処理。</summary>
	void Draw() override;

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


	std::vector<TYEngine::Graphics::Sprite*>& GetSprite() { return setColliderSpr_; }

private:
	void OffsetPos(const TYEngine::Utility::Vector2& pos);

	void ScoreViewSetting();

	void DebugJMApply();

private:
	enum RetryUISprites
	{
		TEN_THOUSANDS = 0,
		THOUSANDS,
		HUNDREDS,
		TENS,
		ONES,
		SpriteNum
	};

	std::array<std::unique_ptr<TYEngine::Graphics::Sprite>, SpriteNum> sprites_;

	int32_t prevScore_ = 0;
	int32_t currentScore_ = 0;
	int32_t viewScore_ = 0;

	struct ScoreDisplay
	{
		std::array<int32_t, SpriteNum> num;
	};
	ScoreDisplay scoreDisplay_ = {};

	std::vector<TYEngine::Graphics::Sprite*> setColliderSpr_;

	float scoreViewTimer_ = 0;
	float scoreViewTime_ = 0.0f;

	float deltaTime_ = 0.0f;
};

