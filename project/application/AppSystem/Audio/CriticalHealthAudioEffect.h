#pragma once
#include <wrl.h>

namespace TYEngine
{
    namespace AudioSystem
    {
        class MyAnalyzerXAPO;
    }

    /// <summary>
    /// HP低下時のピンチ演出（こもったサウンド＋心拍リズム＋ヴィネット効果）を制御するクラス。
    /// </summary>
    class CriticalHealthAudioEffect
    {
    public:
        CriticalHealthAudioEffect();
        ~CriticalHealthAudioEffect() = default;

        /// <summary>
        /// 毎フレームの更新処理。
        /// HPの低下具合に応じて、サウンドのイコライザー値と画面のヴィネット効果を同期して更新する。
        /// </summary>
        /// <param name="deltaTime">前フレームからの経過時間</param>
        /// <param name="healthPercent">プレイヤーのHP割合（0.0～1.0）</param>
        /// <param name="analyzer">イコライザー制御対象のXAPOインスタンス</param>
        void Update(float deltaTime, float healthPercent, Microsoft::WRL::ComPtr<AudioSystem::MyAnalyzerXAPO> analyzer);

    private:
        /// <summary>心拍演出のフェーズ（0.0～1.0）</summary>
        float heartbeatPhase_ = 0.0f;
    };
}
