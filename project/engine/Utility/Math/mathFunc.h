#pragma once
#include "struct.h"
#include <vector>

/// <summary>
/// 法線ベクトルを行列で変換する
/// </summary>
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

/// <summary>
/// 3次元ベクトルの長さ（ノルム）を求める
/// </summary>
float Length(const Vector3& v);

/// <summary>
/// 2次元ベクトルの長さ（ノルム）を求める
/// </summary>
float Length(const Vector2& v);

/// <summary>
/// 3次元ベクトルを正規化する
/// </summary>
Vector3 Normalize(const Vector3& v);

/// <summary>
/// 2次元ベクトルを正規化する
/// </summary>
Vector2 Normalize(const Vector2& v);

/// <summary>
/// 3次元ベクトルの内積を求める
/// </summary>
float Dot(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 3次元ベクトルの減算を行う (v1 - v2)
/// </summary>
Vector3 Subtruct(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 3次元ベクトルの加算を行う (v1 + v2)
/// </summary>
Vector3 Add(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 3次元ベクトル同士の要素ごとの積を求める
/// </summary>
const Vector3 Multiply(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 3次元ベクトルにスカラーを掛ける
/// </summary>
const Vector3 Multiply(const Vector3& v, float s);

/// <summary>
/// スカラーに3次元ベクトルを掛ける
/// </summary>
const Vector3 Multiply(float s, const Vector3& v);

/// <summary>
/// スカラー値を線形補間する
/// </summary>
float Lerp(float a, float b, float t);

/// <summary>
/// 2次元ベクトルを線形補間する
/// </summary>
Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);

/// <summary>
/// 3次元ベクトルを線形補間する
/// </summary>
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

/// <summary>
/// 3次元ベクトルを球面線形補間（Slerp）する
/// </summary>
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

/// <summary>
/// Catmull-Rom補間を使用してベクトルを求める
/// </summary>
Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

/// <summary>
/// 制御点リストからCatmull-Romスプライン上の位置を求める
/// </summary>
Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

/// <summary>
/// インデックスを配列サイズでラップさせる（負数にも対応）
/// </summary>
size_t WrapIndex(ptrdiff_t i, size_t n);

/// <summary>
/// 閉じたCatmull-Romスプライン上の位置を求める
/// </summary>
Vector3 CatmullRomPositionClosed(const std::vector<Vector3>& points, float t);

/// <summary>
/// 3次元ベクトルの外積を求める
/// </summary>
Vector3 Cross(const Vector3& v1, const Vector3& v2);

/// <summary>
/// OBB（方向付き境界ボックス）の回転を適用する
/// </summary>
void OBBRotation(const Vector3& rotate, OBB& obb);

/// <summary>
/// 3次元ベクトルの加算演算子
/// </summary>
Vector3 operator+(const Vector3& a, const Vector3& b);

/// <summary>
/// 3次元ベクトルの減算演算子
/// </summary>
Vector3 operator-(const Vector3& a, const Vector3& b);

/// <summary>
/// 3次元ベクトルの符号反転演算子
/// </summary>
Vector3 operator-(const Vector3& a);

/// <summary>
/// 3次元ベクトル同士の要素ごとの乗算
/// </summary>
Vector3 operator*(const Vector3& a, const Vector3& b);

/// <summary>
/// 3次元ベクトルにスカラーを掛ける
/// </summary>
Vector3 operator*(const Vector3& v, float s);

/// <summary>
/// スカラーに3次元ベクトルを掛ける
/// </summary>
Vector3 operator*(float s, const Vector3& v);

/// <summary>
/// 3次元ベクトル同士の要素ごとの除算
/// </summary>
Vector3 operator/(const Vector3& a, const Vector3& b);

/// <summary>
/// 3次元ベクトルをスカラーで割る
/// </summary>
Vector3 operator/(const Vector3& v, float s);

/// <summary>
/// 2次元ベクトルの減算演算子
/// </summary>
Vector2 operator-(const Vector2& a, const Vector2& b);

/// <summary>
/// 2次元ベクトルの加算演算子
/// </summary>
Vector2 operator+(const Vector2& a, const Vector2& b);

/// <summary>
/// 2次元ベクトル同士の要素ごとの乗算
/// </summary>
Vector2 operator*(const Vector2& a, const Vector2& b);

/// <summary>
/// 2次元ベクトルにスカラーを掛ける
/// </summary>
Vector2 operator*(const Vector2& v, float s);

/// <summary>
/// スカラーに2次元ベクトルを掛ける
/// </summary>
Vector2 operator*(float s, const Vector2& v);

/// <summary>
/// 2次元ベクトル同士の要素ごとの除算
/// </summary>
Vector2 operator/(const Vector2& a, const Vector2& b);

/// <summary>
/// 2次元ベクトルをスカラーで割る
/// </summary>
Vector2 operator/(const Vector2& v, float s);

