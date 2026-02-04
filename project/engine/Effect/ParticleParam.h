#pragma once
#include "struct.h"

namespace TYEngine {
namespace Effect {

using namespace Utility;

/// <summary>
/// 個々のパーティクルが持つパラメータ構造体。
/// 位置、速度、色、寿命などを管理する。
/// </summary>
struct ParticleParam
{
    Transform transform; ///< 位置・回転・スケール。
    Vector3 velocity;    ///< 速度ベクトル。
    Vector4 color;       ///< 色（RGBA）。
    float lifeTime = 0.0f;    ///< 全寿命（秒）。
    float currentTime = 0.0f; ///< 経過時間（秒）。
};

} // namespace Effect
} // namespace TYEngine
