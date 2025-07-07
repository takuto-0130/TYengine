#pragma once
#include <functional>

class ITransition 
{
public:
	virtual ~ITransition() = default;

	virtual void Init() = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	virtual bool IsFinished() const = 0;

	// 通知設定
	void SetOnFinishCallback(std::function<void()> callback) {
		onFinishCallback_ = std::move(callback);
	}

	// TransitionManagerで呼び出し（完了時に通知）
	void NotifyFinished() {
		if (onFinishCallback_) {
			onFinishCallback_();
			onFinishCallback_ = nullptr; // 1回限りにする
		}
	}

private:
	std::function<void()> onFinishCallback_;
};

// コールバックの使用例
/*
auto transition = std::make_unique<Transition>(Transition::Type::ENTER, 1.0f);

transition->SetOnFinishCallback([]() {
	sceneManager_->ChangeScene("GAME");
	});

transitionManager.Start(std::move(transition));
*/