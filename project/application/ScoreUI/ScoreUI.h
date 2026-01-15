#pragma once
#include "Sprite.h"

#include "Utils/Json/JsonManager.h"

#include <memory>
class ScoreUI
{
public:
	void Init();

	void Update();

	void Draw();

	void UpdateResult(float currentTime);

	void SetScore(int32_t score)
	{
		// 追加：現在の目標スコアと同じなら何もしない
		if (currentScore_ == score) return;

		scoreViewTimer_ = 0;
		prevScore_ = currentScore_;
		currentScore_ = score;
	}

	// 仮対応
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

