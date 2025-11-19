// ConfettiSystem.h
#pragma once
#include "ConfettiParticle.h"
#include "Collision2D.h"
#include <vector>

class ConfettiSystem
{
public:
    void Init(int maxCount, const std::string& texturePath,
        int screenW, int screenH);

    void Burst(int count);
    void Emit(int countPerFrame);

    void Update(float dt);
    void Draw();

    // UI AABB を登録（ポインタで受けて参照だけ持つ）
    void SetUIColliders(const std::vector<AABB2D>& uiAABBs)
    {
        uiAABBs_ = &uiAABBs;
    }

private:
    void SpawnOne();
    void SpawnShatter(const Vector2& pos, const Vector4& color, float depth, int count);
    bool CheckAndHandleHitUI(ConfettiParticle& p, const Vector2& drawPos);

private:
    std::vector<ConfettiParticle> particles_;

    int screenW_ = 0;
    int screenH_ = 0;
    std::string texturePath_;

    float globalTime_ = 0.0f; // 風揺れ用の経過時間

    // UI との衝突用
    const std::vector<AABB2D>* uiAABBs_ = nullptr;
};
