#pragma once
class ComboManager
{
public:
	~ComboManager() = default;

	void Init();
	void Update();

	void OnEnemyDefeated();

public:
	int GetComboCount() const { return comboCount_; }
	float GetCurrentComboTimer() const { return comboTimer_; }
	float GetStartComboTime() const { return kComboTime_; }
	float GetCameraShakeTime() const { return shakeTime_; }

private:
	int comboCount_ = 0;
	float comboTimer_ = 0;
	float kComboTime_ = 3.0f;
	float shakeTime_ = 0.4f;
};

