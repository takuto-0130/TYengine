#pragma once
#include <unordered_map>
#include <functional>
#include <optional>
#include <cassert>
#include <string>
#include <imgui.h>
/// --------------------------------------------------------------------------------- ///
/// !!!!!!!!!!!!!!!!!!!!!!!継承より委譲の方が適切なようなので後に修正!!!!!!!!!!!!!!!!!!!!!!! ///
/// --------------------------------------------------------------------------------- ///



// クラスの型 'C' に 'StateFunctionSet' が定義されていて、
// 'C::GetStateTable()' が返す型が 'std::vector<C::StateFunctionSet>' であることを要求するコンセプト
template<typename C>
concept HasStateTable = requires
{
    { C::GetStateTable() } -> std::same_as<const std::vector<typename C::StateFunctionSet>&>;
};

// 汎用ステートマシンクラス（テンプレート）
// 'Class' = 継承先クラス  
// 'StateEnum' = 継承先ごと固有のステートのenumClass
// 使い方はStateMachine.hに
template<typename Class, typename StateEnum>
class StateMachine
{
public:
    using State = StateEnum;
    using StateFunc = void (Class::*)();

    // 関数テーブル用
    // 要素：{ StateEnum, InitFunc, UpdateFunc, ExitFunc }
    struct StateFunctionSet 
    {
        State state;
        StateFunc init, update, exit;
    };

public: // メンバ関数
    /// <summary>
    /// 関数テーブル登録用
    /// </summary>
    /// <param name="instance"> 継承先時クラスのインスタンス </param>
    template<HasStateTable C>
    void RegisterFromDefaultTable(C* instance) 
    {
        for (const auto& entry : C::GetStateTable()) 
        {
            SetInitFunction(entry.state, [instance, f = entry.init]() { (instance->*f)(); });
            SetUpdateFunction(entry.state, [instance, f = entry.update]() { (instance->*f)(); });
            SetExitFunction(entry.state, [instance, f = entry.exit]() { (instance->*f)(); });
        }
    }

    // ステート変更リクエストの呼び出し
    void ChangeState(State next) 
    {
        if (stateRequest_ != next) 
        {
            stateRequest_ = next;
        }
    }

    // ステートのアップデート
    void UpdateState(float deltaTime) 
    {
        if (stateRequest_ && allowExit_)
        {
            if (exitTable_.contains(currState_)) 
            {
                exitTable_[currState_]();
            }
            prevState_ = currState_;
            currState_ = *stateRequest_;
            stateRequest_ = std::nullopt;
            stateTimer_ = 0.0f;
            if (initTable_.contains(currState_)) 
            {
                initTable_[currState_]();
            }
        }

        stateTimer_ += deltaTime;

        if (updateTable_.contains(currState_)) 
        {
            updateTable_[currState_]();
        }
    }

    // デバッグ用のImGui表示
    void DebugImGui(const char* labelPrefix = "State") const 
    {
        ImGui::Text("%s: %s", labelPrefix, GetStateName(currState_).c_str());
        ImGui::Text("Elapsed: %.2f sec", stateTimer_);
    }

    // 現在のステートを取得
    State GetCurrentState() const { return currState_; }
    // 前回のステートを取得
    State GetPreviousState() const { return prevState_; }
    // 現在のステートの経過時間を取得
    float GetStateElapsedTime() const { return stateTimer_; }

    // 現在のステートで固定
    void LockState() { allowExit_ = false; }
    // 現在のステートの固定を解除
    void UnlockState() { allowExit_ = true; }

protected:
    // オーバーライドで列挙名を文字列化（ImGui表示用）
    virtual std::string GetStateName(State state) const = 0;

private: // 関数テーブルsetter
    void SetInitFunction(State state, std::function<void()> func) { initTable_[state] = func; }

    void SetUpdateFunction(State state, std::function<void()> func) { updateTable_[state] = func; }

    void SetExitFunction(State state, std::function<void()> func) { exitTable_[state] = func; }

private: // メンバ変数
    State currState_{};
    State prevState_{};
    std::optional<State> stateRequest_;
    float stateTimer_ = 0.0f;
    bool allowExit_ = true;

    // 関数テーブル
    std::unordered_map<State, std::function<void()>> initTable_;
    std::unordered_map<State, std::function<void()>> updateTable_;
    std::unordered_map<State, std::function<void()>> exitTable_;
};

// StateMachine.h
// 関数テーブルの登録用マクロ
#define STATE_ENTRY_FOR(cls, stateEnum, funcName) \
    { stateEnum, &cls::Init##funcName, &cls::Update##funcName, &cls::Exit##funcName }


// 使い方
// 更新処理の中で UpdateState(float deltaTime) を呼び出し、ステートを切り替えたいときに適宜ChangeState(State next)を呼び出す
// 以下宣言の例

#if 0

//.h
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
        RegisterFromDefaultTable(this);
    }

    static const std::vector<StateFunctionSet>& GetStateTable();

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

//.cpp
#define CLASS_ENTRY(stateEnum, funcName) \
    STATE_ENTRY_FOR(Class, stateEnum, funcName)

const std::vector<StateMachine<Class, State>::StateFunctionSet>& Class::GetStateTable()
{
    using enum State;
    static const std::vector<StateFunctionSet> stateTable = {
        CLASS_ENTRY(ONE, One),
        CLASS_ENTRY(TWO, Two),
        CLASS_ENTRY(THREE, Three),
    };
    return stateTable;
}

#endif // 0
