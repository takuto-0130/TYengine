#pragma once
#include "Sprite.h"

#include "Utils/Json/JsonManager.h"

#include <memory>
#include <random>

class Input;
class ScoreUI;
/// <summary>
/// ゲームプレイ中のUI（HUD）を管理するクラス。
/// スコア、コンボ、HP、操作ガイドなどの表示を行う。
/// </summary>
class PlayUI
{
public:
	/// <summary>初期化処理。スプライトの生成など。</summary>
	void Init();
	/// <summary>更新処理。</summary>
	void Update();
	/// <summary>
	/// 描画処理（通常描画）。
	/// </summary>
	void Draw();

	/// <summary>
	/// レンダーターゲットへの描画が必要な場合に使用。
	/// </summary>
	void DrawRT();

	/// <summary>コンボ数のテクスチャUV更新処理。</summary>
	void ComboTexUpdate();

public:
	/// <summary>コンボタイマーの表示値を設定。</summary>
	void SetComboTimer(float timer) { comboTimer_ = timer; }
	/// <summary>コンボの最大時間を設定（ゲージ表示用）。</summary>
	void SetComboTime(float time) { kComboTime_ = time; }
	/// <summary>コンボ数の表示値を設定。</summary>
	void SetComboNum(int comboNum) { sprites_[COMBO_NUM_TEXT]->SetTextureLeftTop({ sprites_[COMBO_NUM_TEXT]->GetSize().x * float(comboNum),0 }); }
	/// <summary>スコア描画クラスへの参照を設定。</summary>
	void SetScoreDraw(ScoreUI* scoreDraw) { scoreDraw_ = scoreDraw; }

	/// <summary>HPの数値表示を設定。</summary>
	void SetHPNum(int hp = 0) { sprites_[HP_NUM_TEXT]->SetTextureLeftTop({ sprites_[HP_NUM_TEXT]->GetSize().x * float(hp),0 }); }

	/// <summary>シフトガイドの位置を設定。</summary>
	void SetShiftPos(const Vector2& pos);

	/// <summary>ジャスト回避演出の有無を設定。</summary>
	void SetJust(bool just) { isJust_ = just; }

	/// <summary>JSONマネージャを設定（デバッグ調整用）。</summary>
	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

private:
	/// <summary>JSONからパラメータを適用する。</summary>
	void DebugJMApply();

private:
	/// <summary>入力管理クラス。</summary>
	Input* input_ = nullptr;
	/// <summary>スコア描画クラス。</summary>
	ScoreUI* scoreDraw_ = nullptr;

	/// <summary>スプライト配列のインデックス定義。</summary>
	enum PlayUISprites
	{
		COMBO_TEXT,
		COMBO_NUM_TEXT,
		HP_TEXT,
		HP_NUM_TEXT,
		OPERATION,
		OUTLINE,
		PAUSE,
		SHIFT,
		SpriteNum
	};

	/// <summary>UIスプライト群。</summary>
	std::array<std::unique_ptr<Sprite>, SpriteNum> sprites_;

	/// <summary>レティクルスプライト。</summary>
	std::unique_ptr<Sprite> reticle_;

	/// <summary>コンボ表示中のタイマー。</summary>
	float comboTimer_ = 0;
	/// <summary>コンボ最大時間。</summary>
	float kComboTime_ = 0;

	/// <summary>ジャスト回避フラグ。</summary>
	bool isJust_ = false;

	/// <summary>乱数生成器。</summary>
	std::random_device seedGene_;
	/// <summary>シェイク時間。</summary>
	float shakeTime_ = 0.0f;

	/// <summary>JSONマネージャ。</summary>
	jx::JsonManager* jm_;
};

