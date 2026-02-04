#pragma once
#include "struct.h"
#include <cmath>
#include <assert.h>

namespace TYEngine {
namespace Utility {

/// <summary>
/// 2つの行列の加算を行う。
/// </summary>
/// <param name="matrix1">左項の行列。</param>
/// <param name="matrix2">右項の行列。</param>
/// <returns>各要素を加算した結果の行列。</returns>
Matrix4x4 Add(const Matrix4x4& matrix1, const Matrix4x4& matrix2);


/// <summary>
/// 2つの行列の減算を行う。
/// </summary>
/// <param name="matrix1">左項の行列。</param>
/// <param name="matrix2">右項の行列。</param>
/// <returns>各要素を減算した結果の行列。</returns>
Matrix4x4 Subtract(const Matrix4x4& matrix1, const Matrix4x4& matrix2);


/// <summary>
/// 2つの行列の乗算を行う。
/// </summary>
/// <param name="matrix1">左項の行列。</param>
/// <param name="matrix2">右項の行列。</param>
/// <returns>行列の積。</returns>
Matrix4x4 Multiply(const Matrix4x4& matrix1, const Matrix4x4& matrix2);


/// <summary>
/// 行列の逆行列を求める。
/// </summary>
/// <param name="m">対象の行列。</param>
/// <returns>逆行列。</returns>
Matrix4x4 Inverse(const Matrix4x4& m);


/// <summary>
/// 行列の転置を求める。
/// </summary>
/// <param name="matrix">対象の行列。</param>
/// <returns>転置行列。</returns>
Matrix4x4 Transpose(const Matrix4x4& matrix);


/// <summary>
/// 単位行列（恒等変換）を生成する。
/// </summary>
/// <returns>単位行列。</returns>
Matrix4x4 MakeIdentity4x4();


/// <summary>
/// X軸回りの回転行列を生成する。
/// </summary>
/// <param name="radian">回転角度（ラジアン）。</param>
/// <returns>X軸回転行列。</returns>
Matrix4x4 MakeRotateXMatrix(float radian);


/// <summary>
/// Y軸回りの回転行列を生成する。
/// </summary>
/// <param name="radian">回転角度（ラジアン）。</param>
/// <returns>Y軸回転行列。</returns>
Matrix4x4 MakeRotateYMatrix(float radian);


/// <summary>
/// Z軸回りの回転行列を生成する。
/// </summary>
/// <param name="radian">回転角度（ラジアン）。</param>
/// <returns>Z軸回転行列。</returns>
Matrix4x4 MakeRotateZMatrix(float radian);


/// <summary>
/// XYZ軸順の回転行列を生成する。
/// </summary>
/// <param name="rotate">各軸の回転角度（ラジアン単位）。</param>
/// <returns>合成回転行列。</returns>
Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotate);


/// <summary>
/// スケール・回転・平行移動を組み合わせたアフィン変換行列を生成する。
/// </summary>
/// <param name="scale">スケール。</param>
/// <param name="rotate">回転（ラジアン単位）。</param>
/// <param name="translate">平行移動。</param>
/// <returns>アフィン変換行列。</returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);


/// <summary>
/// スケール・クォータニオン回転・平行移動を組み合わせたアフィン変換行列を生成する。
/// </summary>
/// <param name="scale">スケール。</param>
/// <param name="rotationQ">回転を表すクォータニオン。</param>
/// <param name="translation">平行移動。</param>
/// <returns>アフィン変換行列。</returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotationQ, const Vector3& translation);


/// <summary>
/// 透視投影行列（Perspective Projection）を生成する。
/// </summary>
/// <param name="fovY">Y方向の視野角（ラジアン）。</param>
/// <param name="aspectRatio">アスペクト比（幅 / 高さ）。</param>
/// <param name="nearClip">ニアクリップ面距離。</param>
/// <param name="farClip">ファークリップ面距離。</param>
/// <returns>透視投影行列。</returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);


/// <summary>
/// 正射影行列（Orthographic Projection）を生成する。
/// </summary>
/// <param name="left">左端座標。</param>
/// <param name="top">上端座標。</param>
/// <param name="right">右端座標。</param>
/// <param name="bottom">下端座標。</param>
/// <param name="nearClip">ニアクリップ面距離。</param>
/// <param name="farClip">ファークリップ面距離。</param>
/// <returns>正射影行列。</returns>
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);


/// <summary>
/// ビューポート変換行列を生成する。
/// </summary>
/// <param name="left">ビューポート左端座標。</param>
/// <param name="top">ビューポート上端座標。</param>
/// <param name="width">ビューポート幅。</param>
/// <param name="height">ビューポート高さ。</param>
/// <param name="minDepth">最小深度値。</param>
/// <param name="maxDepth">最大深度値。</param>
/// <returns>ビューポート変換行列。</returns>
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);


/// <summary>
/// 平行移動行列を生成する。
/// </summary>
/// <param name="translate">平行移動ベクトル。</param>
/// <returns>平行移動行列。</returns>
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);


/// <summary>
/// スケーリング行列を生成する。
/// </summary>
/// <param name="scale">スケールベクトル。</param>
/// <returns>スケーリング行列。</returns>
Matrix4x4 MakeScaleMatrix(const Vector3& scale);


/// <summary>
/// ベクトルを行列で変換する。
/// </summary>
/// <param name="vector">変換するベクトル。</param>
/// <param name="matrix">適用する行列。</param>
/// <returns>変換後のベクトル。</returns>
Vector3 TransformM(const Vector3& vector, const Matrix4x4& matrix);


/// <summary>
/// カメラのビュー行列からビルボード行列を生成する。
/// 常にカメラに正面を向ける行列を作成する。
/// </summary>
/// <param name="cameraView">カメラのビュー行列。</param>
/// <returns>ビルボード行列。</returns>
Matrix4x4 MakeBillboardMatrix(const Matrix4x4& cameraView);


/// <summary>
/// 行列同士の乗算演算子を定義する。
/// </summary>
/// <param name="m1">左項の行列。</param>
/// <param name="m2">右項の行列。</param>
/// <returns>行列の積。</returns>
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

} // namespace Utility
} // namespace TYEngine

