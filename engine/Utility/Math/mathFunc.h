#pragma once
#include "struct.h"
#include <vector>

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

float Length(const Vector3& v);

float Length(const Vector2& v);

Vector3 Normalize(const Vector3& v);

float Dot(const Vector3& v1, const Vector3& v2);

Vector3 Subtruct(const Vector3& v1, const Vector3& v2);

Vector3 Add(const Vector3& v1, const Vector3& v2);

const Vector3 Multiply(const Vector3& v1, const Vector3& v2);

const Vector3 Multiply(const Vector3& v, const float& s);

const Vector3 Multiply(const float& s, const Vector3& v);

float Lerp(const float& a, const float& b, float t);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);

Vector3 CatmullRomInterpolation(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

Vector3 CatmullRomPosition(const std::vector<Vector3>& points, float t);

Vector3 Cross(const Vector3& v1, const Vector3& v2);



Vector3 operator+(const Vector3& a, const Vector3& b);

Vector3 operator-(const Vector3& a, const Vector3& b);
Vector3 operator-(const Vector3& a);

Vector3 operator*(const Vector3& a, const Vector3& b);
Vector3 operator*(const Vector3& v, const float& s);
Vector3 operator*(const float& s, const Vector3& v);

Vector3 operator/(const Vector3& a, const Vector3& b);
Vector3 operator/(const Vector3& v, const float& s);


Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& v, const float& s);
Vector2 operator/(const Vector2& v, const float& s);
