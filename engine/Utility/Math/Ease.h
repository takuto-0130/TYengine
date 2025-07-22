#pragma once

// イージング関数
namespace Ease {
    // 線形 t = t
    float Linear(float t);

    /// <summary>
    /// 加速 : t^n
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    /// <param name="n"> : 補間カーブの指数（例: 2.0fで2次、3.0fで3次）</param>
    float EaseIn(float t, float n = 2.0f);

    /// <summary>
    /// 減速 : t^n
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    /// <param name="n"> : 補間カーブの指数（例: 2.0fで2次、3.0fで3次）</param>
    float EaseOut(float t, float n = 2.0f);

    /// <summary>
    /// 加速→減速 : t^n
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    /// <param name="n"> : 補間カーブの指数（例: 2.0fで2次、3.0fで3次）</param>
    float EaseInOut(float t, float n = 2.0f);



    /// <summary>
    /// バック補間（EaseIn）: 少し逆方向に引いてから加速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseInBack(float t);

    /// <summary>
    /// バック補間（EaseOut）: 減速しつつ終点を少しオーバーして戻る動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseOutBack(float t);

    /// <summary>
    /// バック補間（EaseInOut）: 最初に引いてから加速し、終盤にオーバーシュートして減速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseInOutBack(float t);



    /// <summary>
    /// 弾性補間（EaseIn）: 始点で跳ねながら加速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseInElastic(float t);

    /// <summary>
    /// 弾性補間（EaseOut）: 終点で跳ねながら減速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseOutElastic(float t);

    /// <summary>
    /// 弾性補間（EaseInOut）: 始点と終点でバネのように跳ねる動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseInOutElastic(float t);



    /// <summary>
    /// バウンド補間（EaseIn）: 跳ね返りながら加速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseInBounce(float t);

    /// <summary>
    /// バウンド補間（EaseOut）: 跳ね返りながら減速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseOutBounce(float t);

    /// <summary>
    /// バウンド補間（EaseInOut）: 前半でバウンドして加速し、後半でバウンドして減速する動き
    /// </summary>
    /// <param name="t"> : 0.0～1.0 の正規化時間</param>
    float EaseInOutBounce(float t);



}
