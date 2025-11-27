#pragma once
#include "struct.h"

/// <summary>
/// クォータニオン同士の積を求める。
/// lhs の回転の後に rhs の回転を適用する。
/// </summary>
/// <param name="lhs">左項のクォータニオン。</param>
/// <param name="rhs">右項のクォータニオン。</param>
/// <returns>合成後のクォータニオン。</returns>
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);


/// <summary>
/// 単位クォータニオン（回転なし）を返す。
/// </summary>
/// <returns>(x, y, z, w) = (0, 0, 0, 1) のクォータニオン。</returns>
Quaternion IdentityQuaternion();


/// <summary>
/// クォータニオンの共役を返す。
/// </summary>
/// <param name="quaternion">対象のクォータニオン。</param>
/// <returns>ベクトル成分を反転したクォータニオン。</returns>
Quaternion Conjugate(const Quaternion& quaternion);


/// <summary>
/// クォータニオンのノルム（長さ）を求める。
/// </summary>
/// <param name="quaternion">対象のクォータニオン。</param>
/// <returns>ノルム（スカラー値）。</returns>
float Norm(const Quaternion& quaternion);


/// <summary>
/// 正規化したクォータニオンを返す。
/// </summary>
/// <param name="quaternion">対象のクォータニオン。</param>
/// <returns>正規化済みクォータニオン。</returns>
Quaternion Normalize(const Quaternion& quaternion);


/// <summary>
/// クォータニオンの逆数を返す。
/// 共役をノルムの2乗で割ったもの。
/// </summary>
/// <param name="quaternion">対象のクォータニオン。</param>
/// <returns>逆クォータニオン。</returns>
Quaternion Inverse(const Quaternion& quaternion);


/// <summary>
/// 任意軸回転からクォータニオンを生成する。
/// </summary>
/// <param name="vec">回転軸（正規化されている必要あり）。</param>
/// <param name="theta">回転角度（ラジアン）。</param>
/// <returns>生成されたクォータニオン。</returns>
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vec, const float theta);


/// <summary>
/// クォータニオンを使ってベクトルを回転させる。
/// </summary>
/// <param name="v">回転させるベクトル。</param>
/// <param name="q">回転を表すクォータニオン。</param>
/// <returns>回転後のベクトル。</returns>
Vector3 RotateVector(const Vector3& v, const Quaternion& q);


/// <summary>
/// クォータニオンから回転行列を生成する。
/// </summary>
/// <param name="q">回転を表すクォータニオン。</param>
/// <returns>回転行列 (Matrix4x4)。</returns>
Matrix4x4 MakeRotateMatrix(const Quaternion& q);


/// <summary>
/// クォータニオン同士の内積を求める。
/// </summary>
/// <param name="q0">1つ目のクォータニオン。</param>
/// <param name="q1">2つ目のクォータニオン。</param>
/// <returns>内積値。</returns>
float Dot(const Quaternion& q0, const Quaternion& q1);


/// <summary>
/// クォータニオンにスカラー値を掛ける。
/// </summary>
/// <param name="q">対象のクォータニオン。</param>
/// <param name="f">スカラー値。</param>
/// <returns>スケール後のクォータニオン。</returns>
Quaternion Multiply(const Quaternion& q, float f);


/// <summary>
/// クォータニオン同士の加算を行う。
/// </summary>
/// <param name="q0">1つ目のクォータニオン。</param>
/// <param name="q1">2つ目のクォータニオン。</param>
/// <returns>加算結果のクォータニオン。</returns>
Quaternion Add(const Quaternion& q0, const Quaternion& q1);


/// <summary>
/// クォータニオンの球面線形補間（Slerp）を行う。
/// </summary>
/// <param name="q0">始点クォータニオン。</param>
/// <param name="q1">終点クォータニオン。</param>
/// <param name="t">補間係数（0～1）。</param>
/// <returns>補間後のクォータニオン。</returns>
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);


/// <summary>
/// forwardベクトルとupベクトルから姿勢を表すクォータニオンを生成する。
/// LookAt行列のクォータニオン版。
/// </summary>
/// <param name="forward">前方向ベクトル。</param>
/// <param name="up">上方向ベクトル。</param>
/// <returns>姿勢を表すクォータニオン。</returns>
Quaternion MakeLookRotation(const Vector3& forward, const Vector3& up);


