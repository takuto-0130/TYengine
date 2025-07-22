#include "Quaternion.h"
#include "mathFunc.h"
#include "Matrix4x4Func.h"
#include <cmath>

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion qr{};
	Vector3 qv = { lhs.x,lhs.y,lhs.z };
	Vector3 rv = { rhs.x,rhs.y,rhs.z };
	qr.w = (lhs.w * rhs.w) - Dot(qv, rv);
	Vector3 qrv = Cross(qv, rv) + (rhs.w * qv) + (lhs.w * rv);
	qr.x = qrv.x;
	qr.y = qrv.y;
	qr.z = qrv.z;
	return qr;
}

Quaternion IdentityQuaternion() {
	Quaternion qr{};
	qr = { 0,0,0,1 };
	return qr;
};

Quaternion Conjugate(const Quaternion& quaternion) {
	Quaternion qr{};
	qr = { quaternion.x * -1.0f,quaternion.y * -1.0f,quaternion.z * -1.0f,quaternion.w };
	return qr;
};

float Norm(const Quaternion& quaternion) {
	return std::sqrtf(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

Quaternion Normalize(const Quaternion& quaternion) {
	Quaternion qr{};
	float norm = Norm(quaternion);
	qr = { quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
	return qr;
};

Quaternion Inverse(const Quaternion& quaternion) {
	Quaternion qr{};
	qr = Conjugate(quaternion);
	float norm = Norm(quaternion);
	qr = { qr.x / (norm * norm), qr.y / (norm * norm), qr.z / (norm * norm), qr.w / (norm * norm) };
	return qr;
};

Quaternion MakeRotateAxisAngleQuaternion(const Vector3& vec, const float theta) {
	Quaternion q = IdentityQuaternion();
	Vector3 n = Normalize(vec);
	q.x = n.x * std::sinf(theta / 2.0f);
	q.y = n.y * std::sinf(theta / 2.0f);
	q.z = n.z * std::sinf(theta / 2.0f);
	q.w = std::cosf(theta / 2.0f);
	return q;
}

Vector3 RotateVector(const Vector3& v, const Quaternion& q) {
	Quaternion result = IdentityQuaternion();
	Quaternion qV = { v.x, v.y, v.z, 0 };
	Quaternion qConj = Conjugate(q);
	result = Multiply(Multiply(q, qV), qConj);
	return Vector3(result.x, result.y, result.z);
}

Matrix4x4 MakeRotateMatrix(const Quaternion& q) {
	Matrix4x4 result = MakeIdentity4x4();
	result.m[0][0] = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	result.m[0][1] = 2 * (q.x * q.y + q.w * q.z);
	result.m[0][2] = 2 * (q.x * q.z - q.w * q.y);

	result.m[1][0] = 2 * (q.x * q.y - q.w * q.z);
	result.m[1][1] = 1.0f - 2.0f * (q.x * q.x + q.z * q.z);
	result.m[1][2] = 2 * (q.y * q.z + q.w * q.x);

	result.m[2][0] = 2 * (q.x * q.z + q.w * q.y);
	result.m[2][1] = 2 * (q.y * q.z - q.w * q.x);
	result.m[2][2] = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	return result;
}

float Dot(const Quaternion& q0, const Quaternion& q1) {
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quaternion Multiply(const Quaternion& q, const float f) {
	return { q.x * f, q.y * f, q.z * f, q.w * f };
}

Quaternion Add(const Quaternion& q0, const Quaternion& q1) {
	return { q0.x + q1.x, q0.y + q1.y, q0.z + q1.z, q0.w + q1.w };
}

Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {
	const float EPSILON = 0.0005f;
	float dot = Dot(q0, q1);
	Quaternion q0cul = q0;

	if (dot < 0) {
		q0cul = { -q0.x, -q0.y, -q0.z,-q0.w };
		dot = -dot;
	}

	if (dot >= 1.0f - EPSILON) {
		return Add(Multiply(q0cul, (1.0f - t)), Multiply(q1, t));
	}

	float theta = std::acosf(dot);
	float q0num = std::sinf((1 - t) * theta) / std::sinf(theta);
	float q1num = std::sinf(t * theta) / std::sinf(theta);

	Quaternion result = {
		q0num * q0cul.x + q1num * q1.x,
		q0num * q0cul.y + q1num * q1.y,
		q0num * q0cul.z + q1num * q1.z,
		q0num * q0cul.w + q1num * q1.w
	};

	return result;
}

Quaternion MakeLookRotation(const Vector3& forward, const Vector3& up)
{
	Vector3 f = Normalize(forward);
	Vector3 r = Normalize(Cross(up, f));
	Vector3 u = Cross(f, r);

	Matrix4x4 rot = MakeIdentity4x4();

	rot.m[0][0] = r.x; rot.m[0][1] = r.y; rot.m[0][2] = r.z;
	rot.m[1][0] = u.x; rot.m[1][1] = u.y; rot.m[1][2] = u.z;
	rot.m[2][0] = f.x; rot.m[2][1] = f.y; rot.m[2][2] = f.z;

	// 回転行列 → クォータニオン変換
	Quaternion q{};
	float trace = rot.m[0][0] + rot.m[1][1] + rot.m[2][2];
	if (trace > 0.0f) 
	{
		float s = std::sqrt(trace + 1.0f) * 2.0f;
		q.w = 0.25f * s;
		q.x = (rot.m[2][1] - rot.m[1][2]) / s;
		q.y = (rot.m[0][2] - rot.m[2][0]) / s;
		q.z = (rot.m[1][0] - rot.m[0][1]) / s;
	}
	else {
		if (rot.m[0][0] > rot.m[1][1] && rot.m[0][0] > rot.m[2][2]) 
		{
			float s = std::sqrt(1.0f + rot.m[0][0] - rot.m[1][1] - rot.m[2][2]) * 2.0f;
			q.w = (rot.m[2][1] - rot.m[1][2]) / s;
			q.x = 0.25f * s;
			q.y = (rot.m[0][1] + rot.m[1][0]) / s;
			q.z = (rot.m[0][2] + rot.m[2][0]) / s;
		}
		else if (rot.m[1][1] > rot.m[2][2]) 
		{
			float s = std::sqrt(1.0f + rot.m[1][1] - rot.m[0][0] - rot.m[2][2]) * 2.0f;
			q.w = (rot.m[0][2] - rot.m[2][0]) / s;
			q.x = (rot.m[0][1] + rot.m[1][0]) / s;
			q.y = 0.25f * s;
			q.z = (rot.m[1][2] + rot.m[2][1]) / s;
		}
		else 
		{
			float s = std::sqrt(1.0f + rot.m[2][2] - rot.m[0][0] - rot.m[1][1]) * 2.0f;
			q.w = (rot.m[1][0] - rot.m[0][1]) / s;
			q.x = (rot.m[0][2] + rot.m[2][0]) / s;
			q.y = (rot.m[1][2] + rot.m[2][1]) / s;
			q.z = 0.25f * s;
		}
	}

	return Normalize(q);
}
