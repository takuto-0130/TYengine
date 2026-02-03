#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"

class Sprite;
/// <summary>
/// ゲームオーバー時のリトライUI管理クラス。
/// 画面表示と入力待機演出を行う。
/// </summary>
class RetryUI
{
public:
	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	/// <summary>UI表示演出を開始する。</summary>
	void Start();

	/// <summary>演出中の動作処理。</summary>
	void Move();

	/// <summary>状態をリセットする。</summary>
	void Reset();

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	void DebugJMApply();

private:
	enum RetryUISprites
	{
		GAMEOVER,
		SCORE_TEXT,
		SPACE,
		SpriteNum
	};

	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	float timer_ = 0.0f;

	float maxTime_ = 0.0f;

	jx::JsonManager* jm_;
};

