#pragma once
#include <cstdint>
/// <summary>
/// スコア管理クラス。
/// ゲームスコアの計算・保持を行う。
/// コンボ倍率などを考慮したスコア加算ロジックを持つ。
/// </summary>
class ScoreManager
{
public:
	~ScoreManager() = default;

	/// <summary>初期化処理。スコアをリセットする。</summary>
	void Init();
	/// <summary>毎フレームの更新処理。</summary>
	void Update();

	/// <summary>
	/// スコアを加算する。
	/// </summary>
	/// <param name="multiplier">コンボ倍率などの係数（基本スコア * multiplier が加算される）。</param>
	void AddScore(float multiplier);

public:
	/// <summary>現在のスコアを取得する。</summary>
	int32_t GetScore() const { return score_; }
	/// <summary>
	/// 敵撃破時の基本スコアを取得する。
	/// </summary>
	int32_t GetBasicScore() const { return kBasicScore_; }

private:
	/// <summary>現在のスコア。</summary>
	int32_t score_ = 0;
	/// <summary>基本スコア（定数）。</summary>
	const int32_t kBasicScore_ = 20;
};

