#pragma once
#include "IPAttackStrategy.h"
#include "PAttackStrategies.h"
#include "../Player.h"
#include "../../Enemy/Enemy.h"
#include "../../Enemy/EnemyManager/EnemyManager.h"
#include "BeatAnalyzer.h"
#include "Timer.h"
#include "../../../AppSystem/Audio/GameAudio.h"
#include "Input.h"
#include "../Reticle/Reticle.h"
#include "Camera.h"
#include "../PlayerBullet/HomingBullet/PlayerBulletHoming.h"
#include "../PlayerBullet/NormalBullet/PlayerBulletNormal.h"
#include "../PlayerBullet/PlayerBulletManager.h"
#include <cmath>


// 敵の攻撃
namespace PlayerAttack
{
	// 共通ヘルパー（通常弾）: 弾を1発生成してマネージャに追加
	inline void SpawnBullet(PlayerBulletManager* manager, PlayerBulletType type, const TYEngine::Utility::Vector3& pos, const TYEngine::Utility::Vector3& dir)
	{
		manager->Fire(type, pos, dir);
	}
    class PHomingAttackStrategy : public IPAttackStrategy
    {
    public:
        // --- ホーミング（ロックオン）攻撃のロジック ---
        void Update(Player* p)
        {
            auto& lockOn = p->GetLockOn();
            auto* input = p->GetInput();

            // クールタイムの減算
            if (lockOn.lockOnTimer > 0)
            {
                lockOn.lockOnTimer -= TYEngine::Utility::Timer::GetInstance()->GetRawDeltaTime();
            }

            // 現在の射撃ボタン入力状態 (LCONTROL or 右クリック)
            bool isPressing = input->PushKey(DIK_LCONTROL) || input->IsPressMouse(1);

            // ▼ ロックオン・サーチフェーズ
            if (isPressing && lockOn.lockOnTimer <= 0)
            {
                if (lockOn.lockedEnemies.size() < lockOn.maxLockCount && lockOn.enemyManager)
                {
                    Enemy* target = lockOn.enemyManager->GetBestLockOnTarget(
                        p->GetCamera(), p->GetScreenOffset(), lockOn.lockOnRadius, lockOn.lockedEnemies);

                    if (target)
                    {
                        lockOn.lockedEnemies.push_back(target);
                        GameAudio::GetInstance()->Play("enter", false, SoundCategory::SE);
                        lockOn.lockOnTimer = lockOn.maxLockOnCool;
                    }
                }
            }
            // ▼ 一斉発射フェーズ
            else if (lockOn.wasPressingShot && !isPressing)
            {
                if (!lockOn.lockedEnemies.empty())
                {
                    p->GetBullets().currentBulletType = PlayerBulletType::HOMING;

                    int bulletCount = 0;
                    int totalBullets = static_cast<int>(lockOn.lockedEnemies.size());

                    for (Enemy* target : lockOn.lockedEnemies)
                    {
                        if (lockOn.enemyManager->IsValidEnemy(target))
                        {
                            TYEngine::Utility::Vector3 up = p->GetCamera()->GetUp();
                            TYEngine::Utility::Vector3 right = p->GetCamera()->GetRight();
                            TYEngine::Utility::Vector3 forward = p->GetCamera()->GetForward();

                            float spreadX = (totalBullets > 1) ? -lockOn.spreadX + ((lockOn.spreadX * 2.0f) * bulletCount / (totalBullets - 1)) : 0.0f;
                            float spreadY = lockOn.spreadY;

                            TYEngine::Utility::Vector3 initialDir = Normalize(forward + (right * spreadX) + (up * spreadY));
                            p->GetBullets().bulletManager->Fire(p->GetBullets().currentBulletType, p->GetWorldPosition(), initialDir, target, lockOn.enemyManager);

                            bulletCount++;
                            GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);
                        }
                    }
                    lockOn.lockedEnemies.clear();
                }
            }
            lockOn.wasPressingShot = isPressing;
        }
    };
   
    class PNormalRhythmAttackStrategy : public IPAttackStrategy
    {
    public:
        // --- リズム通常攻撃のロジック ---
        void Update(Player* p)
        {
            auto& bullets = p->GetBullets();
            auto* input = p->GetInput();

            // クールタイムの減算
            if (bullets.bulletTimer > 0)
            {
                bullets.bulletTimer -= TYEngine::Utility::Timer::GetInstance()->GetRawDeltaTime();
            }

            // コンボリセットタイマーの減算
            if (comboResetTimer_ > 0)
            {
                comboResetTimer_ -= TYEngine::Utility::Timer::GetInstance()->GetRawDeltaTime();
                if (comboResetTimer_ <= 0)
                {
                    comboStep_ = 0; // 時間経過でコンボを1段目に戻す
                }
            }

            // ホーミングボタンが押されていない時のみ通常射撃が可能
            bool isHomingPressing = input->PushKey(DIK_LCONTROL) || input->IsPressMouse(1);
            if (!isHomingPressing)
            {
                if ((input->TriggerKey(DIK_SPACE) || input->IsTriggerMouse(0)) && bullets.bulletTimer <= 0)
                {
                    // リズム判定
                    p->RhythmJudgment();
                    bullets.currentBulletType = PlayerBulletType::NORMAL;

                    // 基準となるベクトルの計算
                    TYEngine::Utility::Vector3 origin = p->GetWorldPosition();
                    TYEngine::Utility::Vector3 target = p->GetReticle()->GetTarget();
                    TYEngine::Utility::Vector3 forward = TYEngine::Utility::Normalize(target - origin);

                    TYEngine::Utility::Vector3 worldUp = { 0.0f, 1.0f, 0.0f };
                    if (std::abs(forward.y) > 0.999f) worldUp = { 0.0f, 0.0f, 1.0f };
                    TYEngine::Utility::Vector3 right = TYEngine::Utility::Normalize(Cross(worldUp, forward));
                    TYEngine::Utility::Vector3 up = TYEngine::Utility::Normalize(Cross(forward, right));

                    // ▼ コンボ段階に応じた発射パターンの分岐
                    if (comboStep_ == 0)
                    {
                        // 1段目: 通常の1発（中央）
                        bullets.bulletManager->Fire(bullets.currentBulletType, origin, forward);
                    }
                    else if (comboStep_ == 1)
                    {
                        // 2段目: 左右2連装（水平に少しずらす）
                        TYEngine::Utility::Vector3 dirRight = TYEngine::Utility::Normalize(forward + right * 0.05f);
                        TYEngine::Utility::Vector3 dirLeft = TYEngine::Utility::Normalize(forward - right * 0.05f);

                        bullets.bulletManager->Fire(bullets.currentBulletType, origin, dirRight);
                        bullets.bulletManager->Fire(bullets.currentBulletType, origin, dirLeft);
                    }
                    else if (comboStep_ == 2)
                    {
                        // 3段目: 扇状3連装（中央＋左右広め）
                        TYEngine::Utility::Vector3 dirRight = TYEngine::Utility::Normalize(forward + right * 0.1f);
                        TYEngine::Utility::Vector3 dirLeft = TYEngine::Utility::Normalize(forward - right * 0.1f);

                        bullets.bulletManager->Fire(bullets.currentBulletType, origin, forward);  // 中央
                        bullets.bulletManager->Fire(bullets.currentBulletType, origin, dirRight); // 右
                        bullets.bulletManager->Fire(bullets.currentBulletType, origin, dirLeft);  // 左
                    }
                    else if (comboStep_ >= 3)
                    {
                        auto& lockOn = p->GetLockOn();
                        for(int i = 0; i < maxLockOn_; i++)
                        {
                            if (lockOn.lockedEnemies.size() < maxLockOn_ && lockOn.enemyManager)
                            {
                                Enemy* targetE = lockOn.enemyManager->GetBestLockOnTarget(
                                    p->GetCamera(), p->GetScreenOffset(), lockOn.lockOnRadius, lockOn.lockedEnemies);

                                if (targetE)
                                {
                                    lockOn.lockedEnemies.push_back(targetE);
                                }
                            }
                        }
                        if (!lockOn.lockedEnemies.empty())
                        {
                            p->GetBullets().currentBulletType = PlayerBulletType::HOMING;

                            int bulletCount = 0;
                            int totalBullets = static_cast<int>(lockOn.lockedEnemies.size());

                            for (Enemy* targetE : lockOn.lockedEnemies)
                            {
                                if (lockOn.enemyManager->IsValidEnemy(targetE))
                                {
                                    up = p->GetCamera()->GetUp();
                                    right = p->GetCamera()->GetRight();
                                    forward = p->GetCamera()->GetForward();

                                    float spreadX = (totalBullets > 1) ? -lockOn.spreadX + ((lockOn.spreadX * 2.0f) * bulletCount / (totalBullets - 1)) : 0.0f;
                                    float spreadY = lockOn.spreadY;

                                    TYEngine::Utility::Vector3 initialDir = Normalize(forward + (right * spreadX) + (up * spreadY));
                                    p->GetBullets().bulletManager->Fire(p->GetBullets().currentBulletType, p->GetWorldPosition(), initialDir, targetE, lockOn.enemyManager);

                                    bulletCount++;
                                }
                            }
                            lockOn.lockedEnemies.clear();
                        }
                    }

                    // 発射音
                    GameAudio::GetInstance()->Play("attack", false, SoundCategory::SE);

                    // クールタイム設定
                    bullets.bulletTimer = bullets.bulletCoolTime;

                    comboResetTimer_ = kComboResetTime_;


                    // コンボの更新処理
                    if(p->GetStatus().currentJudgment == HitJudgment::Perfect ||
                        p->GetStatus().currentJudgment == HitJudgment::Good)
                    {
                        comboStep_ = (comboStep_ + 1) % maxCombo_;
                    }
                    else
                    {
                        comboStep_ = 0;
                    }
                }
            }
        }
    private:
        int comboStep_ = 0;
        int maxCombo_ = 4;
        int maxLockOn_ = 5;
        float comboResetTimer_ = 0;
        float kComboResetTime_ = 0.5f;
    };
}