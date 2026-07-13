#include "ComboManager.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif

using namespace TYEngine::Utility;

void HitStreakManager::Init()
{
	jsonManager_ = std::make_unique<JsonManager>();
	std::string err;
	jsonManager_->Load("HitStreak.json", true, &err);

	// 初期値が設定されていない場合のために、デフォルト値を設定
	if (!jsonManager_->Root().contains("comboCap")) {
		jsonManager_->Set("comboCap", 99);
	}
	if (!jsonManager_->Root().contains("kComboTime")) {
		jsonManager_->Set("kComboTime", 10.0f);
	}
	if (!jsonManager_->Root().contains("shakeTime")) {
		jsonManager_->Set("shakeTime", 0.4f);
	}
	jsonManager_->Save();

	// 各変数を初期値にリセット
	comboCount_ = 0;
	comboTimer_ = 0;

	comboCap_ = jsonManager_->Get<int>("comboCap", 99);
	kComboTime_ = jsonManager_->Get<float>("kComboTime", 10.0f);
	shakeTime_ = jsonManager_->Get<float>("shakeTime", 0.4f);
}

void HitStreakManager::Update()
{
#ifdef _DEBUG
	// デバッグ中はJSONの設定値をリアルタイム反映
	comboCap_ = jsonManager_->Get<int>("comboCap", 99);
	kComboTime_ = jsonManager_->Get<float>("kComboTime", 10.0f);
	shakeTime_ = jsonManager_->Get<float>("shakeTime", 0.4f);
#endif

	// コンボタイマーが有効な場合のみカウントダウン
	if (comboTimer_ > 0)
	{
		comboTimer_ -= Timer::GetInstance()->GetDeltaTime();

		// タイマーが0以下になったらコンボ終了とみなしリセット
		if (comboTimer_ <= 0) 
		{
			comboTimer_ = 0;
			comboCount_ = 0;
		}
	}
	DebugDraw();
}

void HitStreakManager::OnEnemyDefeated()
{
	// コンボ数を加算（comboCap_まででキャップ）
	if(comboCount_ < comboCap_)
	{
		comboCount_++;
	}
	// コンボ継続タイマーをリセット（コンボ受付時間を更新）
	comboTimer_ = kComboTime_;
}

void HitStreakManager::DebugDraw() const
{
#ifdef _DEBUG
	// ImGui で編集
	ImGui::Begin("JSON Editor");
	static JsonImGuiEditor inspector(*jsonManager_);
	inspector.Draw(jsonManager_->Root(), "HitStreak.json");
	if (ImGui::Button("SaveUI")) jsonManager_->Save();
	ImGui::End();
#endif // _DEBUG
}
