#pragma once
/// <summary>
/// コンボ管理クラス。
/// コンボ数のカウント、コンボ継続時間の計測、コンボ終了判定を行う。
/// </summary>
class ComboManager
{
public:
	~ComboManager() = default;

	/// <summary>初期化処理。</summary>
	void Init();
	/// <summary>
	/// 毎フレームの更新処理。
	/// コンボタイマーの減算処理などを行う。
	/// </summary>
	void Update();

	/// <summary>
	/// 敵撃破時に呼ばれる。
	/// コンボ数を加算し、タイマーをリセットする。
	/// </summary>
	void OnEnemyDefeated();

public:
	/// <summary>現在のコンボ数を取得する。</summary>
	int GetComboCount() const { return comboCount_; }
	/// <summary>現在のコンボ残り時間を取得する。</summary>
	float GetCurrentComboTimer() const { return comboTimer_; }
	/// <summary>最大コンボ受付時間を取得する。</summary>
	float GetStartComboTime() const { return kComboTime_; }
	/// <summary>カメラシェイクの時間を取得する。</summary>
	float GetCameraShakeTime() const { return shakeTime_; }

private:
	/// <summary>現在のコンボ数。</summary>
	int comboCount_ = 0;
	/// <summary>コンボ持続の残り時間。</summary>
	float comboTimer_ = 0;
	/// <summary>コンボの基本受付時間（定数）。</summary>
	float kComboTime_ = 3.0f;
	/// <summary>コンボ時のシェイク演出時間。</summary>
	float shakeTime_ = 0.4f;
};

