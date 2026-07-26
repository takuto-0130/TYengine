// ConfettiParticle.h
#pragma once
#include "Sprite.h"
#include "struct.h"

/// <summary>
/// 紙吹雪演出用パーティクルの物理運動パラメータ（位置・速度・回転・寿命・風効果）および描画スプライトを保持する構造体。
/// </summary>
struct ConfettiParticle
{
    TYEngine::Graphics::Sprite  sprite;

    TYEngine::Utility::Vector2 pos;           // 物理的な基準位置（揺れの中心）
    TYEngine::Utility::Vector2 vel;           // 速度（左右/上下）

    // 回転
    float   angle = 0.0f;
    float   angularVel = 0.0f;

    // 寿命
    float   life = 0.0f;
    float   maxLife = 0.0f;

    // フワっと感・奥行き
    float   depth = 0.0f; // 0.0 = 奥 / 1.0 = 手前
    float   fallScale = 1.0f; // 落下スケール（軽さ）

    // 風による左右揺れ
    float   swayTime = 0.0f;
    float   swayPhase = 0.0f;
    float   swayFreq = 0.0f;
    float   swayAmp = 0.0f;

    bool    active = false;

    bool isShatter = false;
};
