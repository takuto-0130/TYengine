#include "TransitionManager.h"

TransitionManager::~TransitionManager()
{
	Clear();
}

void TransitionManager::Start(std::unique_ptr<ITransition> transition, bool clearQueue) {
	if (clearQueue) {
		// 既存のキューをクリアして強制的に新しい遷移を開始
		while (!queue_.empty()) queue_.pop();
	}
	current_ = std::move(transition);
	current_->Init();
}

void TransitionManager::Enqueue(std::unique_ptr<ITransition> transition) {
	queue_.push(std::move(transition));
}

void TransitionManager::Update(float deltaTime) {
	if (current_) {
		current_->Update(deltaTime);
		if (current_->IsFinished()) {
			// 完了通知（内部でコールバック実行）
			current_->NotifyFinished();

			// 現在の遷移を破棄して次へ
			current_.reset();
			AdvanceQueue();
		}
	}
}

void TransitionManager::Draw() {
	if (current_) {
		current_->Draw();
	}
}

bool TransitionManager::IsBusy() const {
	return static_cast<bool>(current_);
}

void TransitionManager::Clear() {
	current_.reset();
	while (!queue_.empty()) queue_.pop();
}

void TransitionManager::AdvanceQueue() {
	if (!queue_.empty()) {
		// キューから次の遷移を取り出し開始
		current_ = std::move(queue_.front());
		queue_.pop();
		current_->Init();
		// 1フレーム分の Update を即時実行して初期状態を確定させる
		current_->Update(0.0f);
	}
}
