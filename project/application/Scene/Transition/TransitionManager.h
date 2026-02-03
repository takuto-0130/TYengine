#pragma once
#include "ITransition.h"
#include "SingletonObject.h"
#include <queue>
#include <memory>

/// <summary>
/// 画面遷移（トランジション）演出を統括するマネージャ。
/// キューによる連続再生、シーン切り替え時のフェードイン/アウトなどを管理する。
/// </summary>
class TransitionManager :
	public SingletonObject<TransitionManager>
{
	friend class SingletonObject<TransitionManager>;
	friend struct std::default_delete<TransitionManager>;

private:
	// 外部からの new/delete を禁止
	TransitionManager() = default;
	~TransitionManager();

public:
	/// <summary>
	/// 単発のトランジションを開始（再生）する。
	/// </summary>
	/// <param name="transition">実行するトランジションインスタンス。</param>
	/// <param name="clearQueue">true の場合、既存のキューをクリアして即座に開始する。</param>
	void Start(std::unique_ptr<ITransition> transition, bool clearQueue = true);

	/// <summary>
	/// 連続再生用にトランジションをキューに追加登録する。
	/// </summary>
	/// <param name="transition">追加するトランジションインスタンス。</param>
	void Enqueue(std::unique_ptr<ITransition> transition);

	/// <summary>
	/// 更新処理。
	/// </summary>
	/// <param name="deltaTime">経過時間。</param>
	void Update(float deltaTime);

	/// <summary>
	/// 描画処理。フェード演出などを最前面に描画する。
	/// </summary>
	void Draw();

	/// <summary>
	/// 現在トランジション演出が実行中かどうかを取得する。
	/// </summary>
	/// <returns>実行中なら true。</returns>
	bool IsBusy() const;

	/// <summary>
	/// すべてのトランジションをキャンセル・クリアする。
	/// </summary>
	void Clear();

private:
	/// <summary>待機キューから次のトランジションを取り出し実行する。</summary>
	void AdvanceQueue();

private:
	/// <summary>現在実行中のトランジション。</summary>
	std::unique_ptr<ITransition> current_;
	/// <summary>待機キュー。</summary>
	std::queue<std::unique_ptr<ITransition>> queue_;
};
