#pragma once
#include "struct.h"

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

Quaternion IdentityQuaternion();

Quaternion Conjugate(const Quaternion& quaternion);

float Norm(const Quaternion& quaternion);

Quaternion Normalize(const Quaternion& quaternion);

Quaternion Inverse(const Quaternion& quaternion);

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vec, const float theta);

Vector3 RotateVector(const Vector3& v, const Quaternion& q);

Matrix4x4 MakeRotateMatrix(const Quaternion& q);

float Dot(const Quaternion& q0, const Quaternion& q1);

Quaternion Multiply(const Quaternion& q, const float f);

Quaternion Add(const Quaternion& q0, const Quaternion& q1);

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);

Quaternion MakeLookRotation(const Vector3& forward, const Vector3& up);

