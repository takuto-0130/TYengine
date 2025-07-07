#pragma once
#include "ITransition.h"
#include <queue>
#include <memory>

class TransitionManager {
public:
	static TransitionManager* GetInstance()
	{
		static TransitionManager instance;
		return &instance;
	}

	~TransitionManager();
private:
	TransitionManager() = default;
	TransitionManager(TransitionManager&) = delete;
	TransitionManager& operator=(TransitionManager&) = delete;

public:
	// 単発トランジションの開始（現在のを上書き）
	void Start(std::unique_ptr<ITransition> transition, bool clearQueue = true);

	// 連続再生用に追加登録（queueに積む）
	void Enqueue(std::unique_ptr<ITransition> transition);

	// 更新・描画
	void Update(float deltaTime);
	void Draw();

	// 現在実行中か
	bool IsBusy() const;

	// すべてのトランジションをキャンセル
	void Clear();

private:
	void AdvanceQueue();

private:
	std::unique_ptr<ITransition> current_;
	std::queue<std::unique_ptr<ITransition>> queue_;
};
