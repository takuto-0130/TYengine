#pragma once
#include <memory>

#include "BaseUI.h"

namespace TYEngine::Graphics
{
	class Sprite;
}
/// <summary>
/// ゲームオーバー時のリトライUI管理クラス。
/// 画面表示と入力待機演出を行う。
/// </summary>
class RetryUI : public BaseUI
{
public:
	/// <summary>初期化処理。</summary>
	void Init() override;

	/// <summary>更新処理。</summary>
	void Update() override;

	/// <summary>描画処理。</summary>
	void Draw() override;

	/// <summary>UI表示演出を開始する。</summary>
	void Start();

	/// <summary>演出中の動作処理。</summary>
	void Move();

	/// <summary>状態をリセットする。</summary>
	void Reset();

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

	std::array<std::unique_ptr<TYEngine::Graphics::Sprite>, SpriteNum> sprites_;

	float timer_ = 0.0f;

	float maxTime_ = 0.0f;
};

