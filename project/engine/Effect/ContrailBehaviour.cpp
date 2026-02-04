#include "ContrailBehaviour.h"
#include "IParticleRenderer.h"
#include "Ease.h"

namespace TYEngine {
namespace Effect {

using namespace Utility;

void ContrailBehaviour::Update(ParticleParam& p, float dt)
{
    float t = p.currentTime / p.lifeTime;

    (void)dt;

    // --- ★ 縮む処理（だんだん小さく） ---
    // 時間経過とともにサイズを0に近づける
    float shrink = 1.0f - t;               // 1 → 0 へ
    shrink = max(shrink, 0.0f);            // マイナス防止

    p.transform.scale.x = 0.1f * shrink;
    p.transform.scale.y = 0.1f * shrink;

    // --- ★ 色変化（赤→黄→白） ---
    // 0〜0.3 : 赤→黄
    // 0.3〜1 : 黄→白
    if (t < 0.3f)
    {
        float tt = t / 0.3f; // 0→1
        p.color = Lerp(Vector4(1, 0, 0, 1), Vector4(1, 1, 0, 1), tt); // 赤→黄
    }
    else
    {
        float tt = (t - 0.3f) / 0.7f; // 0→1
        p.color = Lerp(Vector4(1, 1, 0, 1), Vector4(1, 1, 1, 1), Ease::Out(tt, 2.0f)); // 黄→白 (EaseOut)
    }

    // フェードアウト
    p.color.w *= (1.0f - t);
}

} // namespace Effect
} // namespace TYEngine
