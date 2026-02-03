#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"
#include "Input.h"

#include "../../../AppSystem/Audio/GameAudio.h"

class Sprite;

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
class PauseClass
{
public:
	/// <summary>初期化処理。</summary>
	void Init();

	/// <summary>状態のリセット。</summary>
	void Reset();

	/// <summary>毎フレームの更新処理。</summary>
	void Update();

	/// <summary>描画処理。</summary>
	void Draw();

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

	void SetConfigJsonManager(jx::JsonManager* jm) { configJM_ = jm; }

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
		Vector2 pos = {};	
		/// <summary>判定用サイズ。</summary>
		Vector2 size = {};	
		/// <summary>押下状態フラグ。</summary>
		bool isPush = false;
		/// <summary>ホバーアニメーション進捗。</summary>
		float hoverProgress = 0.0f;
		/// <summary>背景バーのスプライト。</summary>
		std::unique_ptr<Sprite> bar;
		/// <summary>音量バー（中身）のスプライト。</summary>
		std::unique_ptr<Sprite> colorBar;
		/// <summary>スライダつまみのスプライト。</summary>
		std::unique_ptr<Sprite> slide;
		/// <summary>テキスト表示用スプライト。</summary>
		std::unique_ptr<Sprite> text;
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
		Vector2 pos = {};	
		/// <summary>判定用サイズ。</summary>
		Vector2 size = {};	
		/// <summary>ボタン画像スプライト。</summary>
		std::unique_ptr<Sprite> button;
		/// <summary>ホバーアニメーション進捗。</summary>
		float hoverProgress = 0.0f;
	};

	std::array<MenuButton, ButtonElements::Num> menuButtons_;

	bool isPush_ = false;
	ButtonElements elements_ = ButtonElements::Num;

	Input* input_ = nullptr;
	GameAudio* audio_ = nullptr;

	float timer_ = 0.0f;


	std::unique_ptr<Sprite> returnTitle_;
	std::unique_ptr<Sprite> resume_;

	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;

	jx::JsonManager* jm_;
	jx::JsonManager* configJM_;
};

