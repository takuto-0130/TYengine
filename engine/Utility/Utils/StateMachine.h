#pragma once
#include <unordered_map>
#include <functional>
#include <optional>
#include <cassert>
#include <string>
#include <imgui.h>

// 汎用ステートマシンクラス（テンプレート）
template<typename StateEnum>
class StateMachine {
public:
    using State = StateEnum;

    // ステート変更リクエストの呼び出し
    void ChangeState(State next) {
        if (stateRequest_ != next) {
            stateRequest_ = next;
        }
    }

    // ステートのアップデート
    void UpdateState(float deltaTime) {
        if (stateRequest_) {
            if (exitTable_.contains(currState_)) {
                exitTable_[currState_]();
            }
            prevState_ = currState_;
            currState_ = *stateRequest_;
            stateRequest_ = std::nullopt;
            stateTimer_ = 0.0f;
            if (enterTable_.contains(currState_)) {
                enterTable_[currState_]();
            }
        }

        stateTimer_ += deltaTime;

        if (updateTable_.contains(currState_)) {
            updateTable_[currState_]();
        }
    }

    // デバッグ用のImGui表示
    void DebugImGui(const char* labelPrefix = "State") const {
        ImGui::Text("%s: %s", labelPrefix, GetStateName(currState_).c_str());
        ImGui::Text("Elapsed: %.2f sec", stateTimer_);
    }

    // 現在のステートを取得
    State GetCurrentState() const { return currState_; }
    // 前回のステートを取得
    State GetPreviousState() const { return prevState_; }
    // 現在のステートの経過時間を取得
    float GetStateElapsedTime() const { return stateTimer_; }

protected:
    void SetEnterFunction(State state, std::function<void()> func) {
        enterTable_[state] = func;
    }

    void SetUpdateFunction(State state, std::function<void()> func) {
        updateTable_[state] = func;
    }

    void SetExitFunction(State state, std::function<void()> func) {
        exitTable_[state] = func;
    }

    // オーバーライドで列挙名を文字列化（ImGui表示用）
    virtual std::string GetStateName(State state) const = 0;

private:
    State currState_{};
    State prevState_{};
    std::optional<State> stateRequest_;
    float stateTimer_ = 0.0f;

    std::unordered_map<State, std::function<void()>> enterTable_;
    std::unordered_map<State, std::function<void()>> updateTable_;
    std::unordered_map<State, std::function<void()>> exitTable_;
};


// 使い方
// 更新処理の中で UpdateState(float deltaTime) を呼び出し、ステートを切り替えたいときに適宜ChangeState(State next)を呼び出す
// 以下宣言の例
/*
enum class State
{
    ONE,
    TWO,
    THREE
};

class Class : public StateMachine<State>
{
public:
    Class()
    {
        SetEnterFunction(State::ONE, [this]() { InitOne(); });
        SetUpdateFunction(State::ONE, [this]() { UpdateOne(); });
        SetExitFunction(State::ONE, [this]() { ExitOne(); });

        SetEnterFunction(State::TWO, [this]() { InitOne(); });
        SetUpdateFunction(State::TWO, [this]() { UpdateOne(); });
        SetExitFunction(State::TWO, [this]() { ExitOne(); });

        SetEnterFunction(State::THREE, [this]() { InitOne(); });
        SetUpdateFunction(State::THREE, [this]() { UpdateOne(); });
        SetExitFunction(State::THREE, [this]() { ExitOne(); });
    }
private:
	std::string GetStateName(State state) const override {
		switch (state) {
		case State::ONE: return "ONE";
		case State::TWO: return "TWO";
		case State::THREE: return "THREE";
		default: return "Unknown";
		}
	}

private:
    void InitOne() {}
    void UpdateOne() {}
    void ExitOne() {}

    void InitTwo() {}
    void UpdateTwo() {}
    void ExitTwo() {}

    void InitThree() {}
    void UpdateThree() {}
    void ExitThree() {}
};
*/
