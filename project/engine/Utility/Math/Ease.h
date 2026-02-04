#pragma once

namespace TYEngine {
namespace Utility {

// イージング関数
namespace Ease 
{

    /// <summary>
    /// 加速 : t^n
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    /// <param name="n"> : 補間カーブの指数（例: 2.0fで2次、3.0fで3次）</param>
    float In(float t, float n = 2.0f);

    /// <summary>
    /// 減速 : t^n
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    /// <param name="n"> : 補間カーブの指数（例: 2.0fで2次、3.0fで3次）</param>
    float Out(float t, float n = 2.0f);

    /// <summary>
    /// 加速→減速 : t^n
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    /// <param name="n"> : 補間カーブの指数（例: 2.0fで2次、3.0fで3次）</param>
    float InOut(float t, float n = 2.0f);
}

namespace EaseFixed
{
    // 線形 t = t
    float Linear(float t);


    /// 加速 : t^2
    inline float InQuad(float t) { return Ease::In(t, 2.0f); }
    /// 減速 : t^2
    inline float OutQuad(float t) { return Ease::Out(t, 2.0f); }
    /// 加速→減速 : t^2
    inline float InOutQuad(float t) { return Ease::InOut(t, 2.0f); }


    /// 加速 : t^3
    inline float InCubic(float t) { return Ease::In(t, 3.0f); }
    /// 減速 : t^3
    inline float OutCubic(float t) { return Ease::Out(t, 3.0f); }
    /// 加速→減速 : t^3
    inline float InOutCubic(float t) { return Ease::InOut(t, 3.0f); }


    /// 加速 : t^4
    inline float InQuart(float t) { return Ease::In(t, 4.0f); }
    /// 減速 : t^4
    inline float OutQuart(float t) { return Ease::Out(t, 4.0f); }
    /// 加速→減速 : t^4
    inline float InOutQuart(float t) { return Ease::InOut(t, 4.0f); }


    /// 加速 : t^5
    inline float InQuint(float t) { return Ease::In(t, 5.0f); }
    /// 減速 : t^5
    inline float OutQuint(float t) { return Ease::Out(t, 5.0f); }
    /// 加速→減速 : t^5
    inline float InOutQuint(float t) { return Ease::InOut(t, 5.0f); }


    /// バック補間（EaseIn）: 少し逆方向に引いてから加速する動き
    float InBack(float t);
    /// バック補間（EaseOut）: 減速しつつ終点を少しオーバーして戻る動き
    float OutBack(float t);
    /// バック補間（EaseInOut）: 最初に引いてから加速し、終盤にオーバーシュートして減速する動き
    float InOutBack(float t);


    /// 弾性補間（EaseIn）: 始点で跳ねながら加速する動き
    float InElastic(float t);
    /// 弾性補間（EaseOut）: 終点で跳ねながら減速する動き
    float OutElastic(float t);
    /// 弾性補間（EaseInOut）: 始点と終点でバネのように跳ねる動き
    float InOutElastic(float t);


    /// バウンド補間（EaseIn）: 跳ね返りながら加速する動き
    float InBounce(float t);
    /// バウンド補間（EaseOut）: 跳ね返りながら減速する動き
    float OutBounce(float t);
    /// バウンド補間（EaseInOut）: 前半でバウンドして加速し、後半でバウンドして減速する動き
    float InOutBounce(float t);
}

} // namespace Utility
} // namespace TYEngine
