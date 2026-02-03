#pragma once

class Enemy;

/// <summary>
/// 敵キャラクターのイベント通知を受け取るためのインターフェース。
/// スコア加算やコンボ処理などのイベントハンドラとして使用される。
/// </summary>
class IEnemyEventListener {
public:
    virtual ~IEnemyEventListener() = default;

    /// <summary>
    /// 敵が死亡（撃破）された際に呼び出される。
    /// </summary>
    /// <param name="enemy">死亡した敵オブジェクトへのポインタ。</param>
    virtual void OnEnemyDied(Enemy* enemy) = 0;
};