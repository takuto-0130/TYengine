#pragma once
#include "StateMachine.h"
#include <functional>

using EaseFunc = std::function<float(float)>;

enum class BulletTimeState
{
	NONE,
	ENTER,	// 補間でスローに入る
	HOLD,	// スロー中
	EXIT,	// 補間で元に戻る
};

class Timer;
// バレットタイム制御用クラス
class BulletTimeController
	: public StateMachine<BulletTimeController, BulletTimeState>
{
public: // 関数テーブル
	static const std::vector<StateFunctionSet>& GetStateTable();

public:
	static BulletTimeController* GetInstance() {
		static BulletTimeController instance;
		return &instance;
	}

	// コピー・ムーブの禁止
	BulletTimeController(const BulletTimeController&) = delete;
	BulletTimeController& operator=(const BulletTimeController&) = delete;
	BulletTimeController(BulletTimeController&&) = delete;
	BulletTimeController& operator=(BulletTimeController&&) = delete;

private:
	BulletTimeController();

public: // メンバ関数

	void Update();

	/// <summary>
	/// スローを発生させるためのトリガー
	/// </summary>
	/// <param name="slowScale">	：スロー倍率</param>
	/// <param name="enterDuration">：設定倍率まで到達する時間</param>
	/// <param name="holdDuration">	：スローを維持する時間</param>
	/// <param name="exitDuration">	：等倍まで戻す時間</param>
	/// <param name="enterEase">	：スローに入る際のイージング</param>
	/// <param name="exitEase">		：スロー終了の際のイージング</param>
	void Trigger(float slowScale, float enterDuration, float holdDuration, float exitDuration, 
		EaseFunc enterEase, EaseFunc exitEase);

	// 終了処理の呼び出し
	void CallStateExit();

	// 手動中断
	void ForceExitNow();

private: // メンバ変数
	struct BulletTimeParams {

		float slowScale = 0.0f;
		float enterDuration = 0.0f;
		float holdDuration = 0.0f;
		float exitDuration = 0.0f;
		EaseFunc enterEase = nullptr;
		EaseFunc exitEase = nullptr;
	};

	BulletTimeParams params_;
	float elapsed_ = 0.0f;

	Timer* timer_ = nullptr;

private: // State関連関数
#pragma region // State関連関数
	std::string GetStateName(BulletTimeState state) const override {
		switch (state) {
		case BulletTimeState::NONE: return "NONE";
		case BulletTimeState::ENTER: return "ENTER";
		case BulletTimeState::HOLD:  return "HOLD";
		case BulletTimeState::EXIT:  return "EXIT";
		default: return "Unknown";
		}
	}

	void InitNone();
	void UpdateNone();
	void ExitNone();

	void InitEnter();
	void UpdateEnter();
	void ExitEnter();

	void InitHold();
	void UpdateHold();
	void ExitHold();

	void InitExit();
	void UpdateExit();
	void ExitExit();
#pragma endregion
};

