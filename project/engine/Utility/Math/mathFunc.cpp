#include "mathFunc.h"
#include "Matrix4x4Func.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <cassert>

namespace TYEngine {
namespace Utility {

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) { 
	Vector3 result{
		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0], 
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1], 
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};
	return result;
}

float Length(const Vector3& v) { 
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }

float Length(const Vector2& v) { 
	return sqrtf(v.x * v.x + v.y * v.y); 
}

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	Vector3 result{v.x / len, v.y / len, v.z / len};
	return result;
}

Vector2 Normalize(const Vector2& v)
{
	float len = Length(v);
	Vector2 result{ v.x / len, v.y / len };
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) { 
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

Vector3 Subtruct(const Vector3& v1, const Vector3& v2) { 
	return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

Vector3 Add(const Vector3& v1, const Vector3& v2) { 
	return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z}; 
}

const Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	return {v1.x * v2.x, v1.y * v2.y, v1.z * v2.z}; 
}

const Vector3 Multiply(const Vector3& v, float s) { 
	return {v.x * s, v.y * s, v.z * s}; 
}

const Vector3 Multiply(float s, const Vector3& v) { 
	return {v.x * s, v.y * s, v.z * s}; 
}

float Lerp(float a, float b, float t) { 
	return a + t * (b - a);
}

Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t)
{
	return v1 + t * (v2 - v1);
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) { 
	return v1 + t * (v2 - v1);
}

Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t)
{
	return v1 + t * (v2 - v1);
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) { 
	Vector3 nV1 = Normalize(v1);
	Vector3 nV2 = Normalize(v2);
	float dot = Dot(nV1, nV2);

	dot = (std::min)(dot, 1.0f);
	float theta = std::acos(dot);
	float sinTheta = std::sin(theta);
	float sinThetaFrom = std::sin((1 - t) * theta);
	float sinThetaTo = std::sin(t * theta);
	Vector3 nSlerp{};
	if (sinTheta < 1.0e-5) {
		nSlerp = nV1;
	} else {
		nSlerp = (sinThetaFrom * nV1 + sinThetaTo * nV2) / sinTheta;
	}
	float length = Lerp(Length(v1), Length(v2), t);

	return length * nSlerp;
}

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) { 
	float s = 0.5f;		// 除算が重いので1/2の代用
	float t2 = t * t;	// tの2乗
	float t3 = t2 * t;	// tの3乗

	Vector3 e3 = -p0 + 3 * p1 - 3 * p2 + p3;
	Vector3 e2 = 2 * p0 - 5 * p1 + 4 * p2 - p3;
	Vector3 e1 = -p0 + p2;
	Vector3 e0 = 2 * p1;

	return s * (e3 * t3 + e2 * t2 + e1 * t + e0);
}

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t) { 
	assert(points.size() >= 4 && "制御点は4点以上必要です");
	size_t division = points.size() - 1;
	float areaWidth = 1.0f / division;
	float t_2 = std::fmod(t, areaWidth) * division;
	t_2 = std::clamp(t_2, 0.0f, 1.0f);

	size_t index = static_cast<size_t>(t / areaWidth);
	index = (std::min)(index, division - 1l);

	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;
	if (index == 0) {
		index0 = index1;
	}
	if (index3 >= points.size()) {
		index3 = index2;
	}
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];
	return CatmullRomInterpolation(p0, p1, p2, p3, t_2);
}

// t は任意の実数（0～1で一周）。負の t も OK。
size_t WrapIndex(ptrdiff_t i, size_t n)
{
	// i が負でも正でも 0..n-1 に丸める
	ptrdiff_t m = i % static_cast<ptrdiff_t>(n);
	if (m < 0) m += static_cast<ptrdiff_t>(n);
	return static_cast<size_t>(m);
}

Vector3 CatmullRomPositionClosed(const std::vector<Vector3>& points, float t)
{
	assert(points.size() >= 4 && "制御点は4点以上必要です");
	const size_t N = points.size();

	// t を [0,1) に正規化（1.0 ちょうどは 0.0 と同じ位置にする）
	float u = t - std::floor(t);        // u in [0,1)
	float fseg = u * static_cast<float>(N);
	size_t seg = static_cast<size_t>(std::floor(fseg)) % N; // 区間の先頭インデックス
	float  lt = fseg - std::floor(fseg);                   // 区間内 t in [0,1)

	// p0..p3 を循環参照で取得
	size_t i0 = WrapIndex(static_cast<ptrdiff_t>(seg) - 1, N);
	size_t i1 = seg;
	size_t i2 = WrapIndex(static_cast<ptrdiff_t>(seg) + 1, N);
	size_t i3 = WrapIndex(static_cast<ptrdiff_t>(seg) + 2, N);

	const Vector3& p0 = points[i0];
	const Vector3& p1 = points[i1];
	const Vector3& p2 = points[i2];
	const Vector3& p3 = points[i3];

	return CatmullRomInterpolation(p0, p1, p2, p3, lt);
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) { 
	return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

void OBBRotation(const Vector3& rotate, OBB& obb) {
	Matrix4x4 OBBrotateMatrix;

	OBBrotateMatrix =
		Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	obb.orientations[0].x = OBBrotateMatrix.m[0][0];
	obb.orientations[0].y = OBBrotateMatrix.m[0][1];
	obb.orientations[0].z = OBBrotateMatrix.m[0][2];

	obb.orientations[1].x = OBBrotateMatrix.m[1][0];
	obb.orientations[1].y = OBBrotateMatrix.m[1][1];
	obb.orientations[1].z = OBBrotateMatrix.m[1][2];

	obb.orientations[2].x = OBBrotateMatrix.m[2][0];
	obb.orientations[2].y = OBBrotateMatrix.m[2][1];
	obb.orientations[2].z = OBBrotateMatrix.m[2][2];
}





Vector3 operator+(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	return result;
}


Vector3 operator-(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

Vector3 operator-(const Vector3& a) {
	Vector3 result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	return result;
}


Vector3 operator*(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
	return result;
}
Vector3 operator*(const Vector3& v, float s) {
	Vector3 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	return result;
}
Vector3 operator*(float s, const Vector3& v) {
	Vector3 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	return result;
}


Vector3 operator/(const Vector3& a, const Vector3& b) {
	Vector3 result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
	return result;
}
Vector3 operator/(const Vector3& v, float s) {
	Vector3 result;
	result.x = v.x / s;
	result.y = v.y / s;
	result.z = v.z / s;
	return result;
}

Vector2 operator-(const Vector2& a, const Vector2& b) {
	Vector2 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

Vector2 operator+(const Vector2& a, const Vector2& b)
{
	Vector2 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}


Vector2 operator*(const Vector2& a, const Vector2& b) {
	Vector2 result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	return result;
}
Vector2 operator*(const Vector2& v, float s)
{
	Vector2 result;
	result.x = v.x * s;
	result.y = v.y * s;
	return result;
}
Vector2 operator*(float s, const Vector2& v)
{
	Vector2 result;
	result.x = v.x * s;
	result.y = v.y * s;
	return result;
}


Vector2 operator/(const Vector2& a, const Vector2& b) {
	Vector2 result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	return result;
}
Vector2 operator/(const Vector2& v, float s)
{
	Vector2 result;
	result.x = v.x / s;
	result.y = v.y / s;
	return result;
}

// --------------------------------------
// Vector4 operator overloads
// --------------------------------------
Vector4 operator+(const Vector4& a, const Vector4& b)
{
	Vector4 result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
	result.w = a.w + b.w;
	return result;
}

Vector4 operator-(const Vector4& a, const Vector4& b)
{
	Vector4 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	result.w = a.w - b.w;
	return result;
}

Vector4 operator-(const Vector4& a)
{
	Vector4 result;
	result.x = -a.x;
	result.y = -a.y;
	result.z = -a.z;
	result.w = -a.w;
	return result;
}

Vector4 operator*(const Vector4& a, const Vector4& b)
{
	Vector4 result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	result.z = a.z * b.z;
	result.w = a.w * b.w;
	return result;
}

Vector4 operator*(const Vector4& v, const float& s)
{
	Vector4 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	result.w = v.w * s;
	return result;
}

Vector4 operator*(const float& s, const Vector4& v)
{
	Vector4 result;
	result.x = v.x * s;
	result.y = v.y * s;
	result.z = v.z * s;
	result.w = v.w * s;
	return result;
}

Vector4 operator/(const Vector4& a, const Vector4& b)
{
	Vector4 result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	result.z = a.z / b.z;
	result.w = a.w / b.w;
	return result;
}

Vector4 operator/(const Vector4& v, const float& s)
{
	Vector4 result;
	result.x = v.x / s;
	result.y = v.y / s;
	result.z = v.z / s;
	result.w = v.w / s;
	return result;
}

} // namespace Utility
} // namespace TYEngine
