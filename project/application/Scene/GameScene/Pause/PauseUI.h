#pragma once
#include <memory>

#include "BaseUI.h"
#include "Input.h"

#include "../../../AppSystem/Audio/GameAudio.h"

namespace TYEngine::Graphics
{
	class Sprite;
}

/// <summary>
/// ポーズメニューのボタンの種類。
/// </summary>
enum ButtonElements
{
	RESUME,			///< ゲームに戻る
	RETURN_TITLE,	///< タイトルに戻る
	Num
};

/// <summary>
/// ポーズ画面の管理クラス。
/// ボタンの操作、音量設定の変更などを担当する。
/// </summary>
class PauseUI : public BaseUI
{
public:
	/// <summary>初期化処理。</summary>
	void Init() override;

	/// <summary>状態のリセット。</summary>
	void Reset();

	/// <summary>毎フレームの更新処理。</summary>
	void Update() override;

	/// <summary>描画処理。</summary>
	void Draw() override;

	void SetConfigJsonManager(TYEngine::Utility::JsonManager* jm) { configJM_ = jm; }

	ButtonElements GetElements() { return elements_; }

private:
	void DebugJMApply();

	void ButtonProcess();

	void VolumeChange();

private:
	struct VolumeControl
	{
		/// <summary>サウンドカテゴリ。</summary>
		SoundCategory soundCategory = SoundCategory::CategoryNum;
		/// <summary>判定用座標。</summary>
		TYEngine::Utility::Vector2 pos = {};
		/// <summary>判定用サイズ。</summary>
		TYEngine::Utility::Vector2 size = {};
		/// <summary>押下状態フラグ。</summary>
		bool isPush = false;
		/// <summary>ホバーアニメーション進捗。</summary>
		float hoverProgress = 0.0f;
		/// <summary>背景バーのスプライト。</summary>
		std::unique_ptr<TYEngine::Graphics::Sprite> bar;
		/// <summary>音量バー（中身）のスプライト。</summary>
		std::unique_ptr<TYEngine::Graphics::Sprite> colorBar;
		/// <summary>スライダつまみのスプライト。</summary>
		std::unique_ptr<TYEngine::Graphics::Sprite> slide;
		/// <summary>テキスト表示用スプライト。</summary>
		std::unique_ptr<TYEngine::Graphics::Sprite> text;
	};

	enum VolumeCategory
	{
		Master,
		BGM,
		SE,
		UI,
		CategoryNum
	};

	std::array<VolumeControl, VolumeCategory::CategoryNum> volumeControl_;

	struct MenuButton
	{
		/// <summary>ボタンの種類。</summary>
		ButtonElements elements = ButtonElements::Num;
		/// <summary>判定用座標。</summary>
		TYEngine::Utility::Vector2 pos = {};
		/// <summary>判定用サイズ。</summary>
		TYEngine::Utility::Vector2 size = {};
		/// <summary>ボタン画像スプライト。</summary>
		std::unique_ptr<TYEngine::Graphics::Sprite> button;
		/// <summary>ホバーアニメーション進捗。</summary>
		float hoverProgress = 0.0f;
	};

	std::array<MenuButton, ButtonElements::Num> menuButtons_;

	bool isPush_ = false;
	ButtonElements elements_ = ButtonElements::Num;

	TYEngine::Framework::Input* input_ = nullptr;
	GameAudio* audio_ = nullptr;

	float timer_ = 0.0f;


	std::unique_ptr<TYEngine::Graphics::Sprite> returnTitle_;
	std::unique_ptr<TYEngine::Graphics::Sprite> resume_;

	std::unique_ptr<TYEngine::Graphics::Sprite> back_;
	std::unique_ptr<TYEngine::Graphics::Sprite> text_;

	TYEngine::Utility::JsonManager* configJM_;
};

