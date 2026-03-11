#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"

namespace TYEngine::Graphics
{
	class Sprite;
}
/// <summary>
/// ゲーム開始時のスタート演出（Ready -> Go）を管理するクラス。
/// </summary>
class StartUI
{
public:
	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	/// <summary>スタート演出を開始する。</summary>
	void Start();

	/// <summary>演出中の動作処理。</summary>
	void Move();

	/// <summary>状態をリセットする。</summary>
	void Reset();

	void SetJsonManager(TYEngine::Utility::JsonManager* jm) { jm_ = jm; }

	void SetGOAlpha(float t);

private:
	void DebugJMApply();

private:
	enum RetryUISprites
	{
		READY,
		GO,
		SpriteNum
	};

	std::array<std::unique_ptr<TYEngine::Graphics::Sprite>, SpriteNum> sprites_;

	float timer_ = 0.0f;

	float maxTime_ = 0.0f;

	TYEngine::Utility::JsonManager* jm_;
};

