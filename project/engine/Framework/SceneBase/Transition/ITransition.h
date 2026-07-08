#pragma once
#include <functional>

/// <summary>
/// 画面遷移（トランジション）演出の基底インターフェース。
/// </summary>
class ITransition 
{
public:
	virtual ~ITransition() = default;

	/// <summary>初期化処理。</summary>
	virtual void Init() = 0;
	/// <summary>更新処理。</summary>
	/// <param name="dt">経過時間。</param>
	virtual void Update(float dt) = 0;
	/// <summary>描画処理。</summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 演出が終了したかどうかを取得する。
	/// </summary>
	/// <returns>終了していれば true。</returns>
	virtual bool IsFinished() const = 0;

	/// <summary>
	/// 完了時に呼ばれるコールバック関数を設定する。
	/// </summary>
	/// <param name="callback">コールバック関数。</param>
	void SetOnFinishCallback(std::function<void()> callback)
	{
		onFinishCallback_ = std::move(callback);
	}

	/// <summary>
	/// 演出完了を通知し、コールバックを実行する。
	/// TransitionManager から呼び出される。
	/// </summary>
	void NotifyFinished()
	{
		if (onFinishCallback_)
		{
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