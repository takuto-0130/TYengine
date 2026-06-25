#include "ComboManager.h"
#include "Timer.h"

using namespace TYEngine::Utility;

void HitStreakManager::Init()
{
	// 各変数を初期値にリセット
	comboCount_ = 0;
	comboTimer_ = 0;
	comboCap_ = 99;
	// コンボ受付時間のデフォルト設定
	kComboTime_ = 3.0f;
	// シェイク演出時間の設定
	shakeTime_ = 0.4f;
}

void HitStreakManager::Update()
{
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
