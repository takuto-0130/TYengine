#pragma once
#include "../../BaseBullet/BaseBullet.h"

/// <summary>
/// 敵の弾を一括管理するクラス。
/// 弾の更新・描画・寿命管理（削除）を行う。
/// </summary>
class EnemyBulletManager
{
public:
	EnemyBulletManager() = default;
	~EnemyBulletManager() = default;

	/// <summary>初期化処理。</summary>
	void Init();
	/// <summary>
	/// 全弾の更新処理。
	/// 寿命切れや衝突済みの弾は削除される。
	/// </summary>
	void Update();
	/// <summary>全弾の描画処理。</summary>
	void Draw();

	/// <summary>管理している全ての弾を消去する。</summary>
	void ClearAll() { bullets_.clear(); }

public:
	/// <summary>
	/// 新しい弾を追加する。
	/// </summary>
	/// <param name="bullet">追加する弾インスタンス（所有権移動）。</param>
	void AddBullet(std::unique_ptr<BaseBullet> bullet) { bullets_.push_back(std::move(bullet)); }

private:
	/// <summary>弾リスト。</summary>
	std::vector<std::unique_ptr<BaseBullet>> bullets_;
};

