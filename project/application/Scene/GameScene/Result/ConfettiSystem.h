// ConfettiSystem.h
#pragma once
#include "ConfettiParticle.h"
#include "Collision2D.h"
#include <vector>

/// <summary>
/// 紙吹雪パーティクルシステム。
/// 大量の紙吹雪を生成、更新、描画し、物理挙動やUIとの衝突を処理する。
/// </summary>
class ConfettiSystem
{
public:
    /// <summary>
    /// 初期化処理。
    /// </summary>
    /// <param name="maxCount">最大パーティクル数。</param>
    /// <param name="texturePath">テクスチャファイルパス。</param>
    /// <param name="screenW">画面幅。</param>
    /// <param name="screenH">画面高さ。</param>
    void Init(int maxCount, const std::string& texturePath,
        int screenW, int screenH);

    /// <summary>
    /// 指定数を一度に放出する（バースト）。
    /// </summary>
    void Burst(int count);

    /// <summary>
    /// 毎フレーム少しずつ放出する。
    /// </summary>
    void Emit(int countPerFrame);

    /// <summary>
    /// 更新処理。
    /// </summary>
    /// <param name="dt">デルタタイム。</param>
    void Update(float dt);

    /// <summary>描画処理。</summary>
    void Draw();

    /// <summary>
    /// UIの衝突判定用AABBリストを登録する。
    /// </summary>
    /// <param name="uiAABBs">AABBリストへの参照。</param>
    void SetUIColliders(const std::vector<TYEngine::Utility::AABB2D>& uiAABBs)
    {
        uiAABBs_ = &uiAABBs;
    }

private:
    void SpawnOne();
    void SpawnShatter(const TYEngine::Utility::Vector2& pos, const TYEngine::Utility::Vector4& color, float depth, int count);
    bool CheckAndHandleHitUI(ConfettiParticle& p, const TYEngine::Utility::Vector2& drawPos);

private:
    /// <summary>パーティクルリスト。</summary>
    std::vector<ConfettiParticle> particles_;

    /// <summary>画面幅。</summary>
    int screenW_ = 0;
    /// <summary>画面高さ。</summary>
    int screenH_ = 0;
    /// <summary>テクスチャパス。</summary>
    std::string texturePath_;

    /// <summary>風揺れ用の経過時間。</summary>
    float globalTime_ = 0.0f; // 風揺れ用の経過時間

    // UI との衝突用
    const std::vector<TYEngine::Utility::AABB2D>* uiAABBs_ = nullptr;
};
