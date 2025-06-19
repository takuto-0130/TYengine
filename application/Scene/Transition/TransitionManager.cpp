#include "TransitionManager.h"

TransitionManager::~TransitionManager()
{
	Clear();
}

void TransitionManager::Start(std::unique_ptr<ITransition> transition, bool clearQueue) {
	if (clearQueue) {
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
		current_ = std::move(queue_.front());
		queue_.pop();
		current_->Init();
		// 1フレーム分の Update/Draw を即時実行
		current_->Update(0.0f);  // ← 状態初期化しておく
	}
}
