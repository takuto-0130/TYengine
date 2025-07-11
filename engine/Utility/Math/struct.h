#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>

struct Vector2 {
	float x;
	float y;
};

struct Vector3 {
	float x;
	float y;
	float z;

	Vector3& operator+=(const Vector3& a) {
		x = a.x + x;
		y = a.y + y;
		z = a.z + z;
		return *this;
	}

	Vector3& operator-=(const Vector3& a) {
		x = x - a.x;
		y = y - a.y;
		z = z - a.z;
		return *this;
	}

	Vector3& operator*=(const float& a) {
		x = x * a;
		y = y * a;
		z = z * a;
		return *this;
	}

	Vector3& operator/=(const float& a) {
		x = x / a;
		y = y / a;
		z = z / a;
		return *this;
	}

	bool operator==(const Vector3& a) const {
		return (x == a.x && y == a.y && z == a.z);
	}
};

struct Vector4 {
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4x4 {
	std::array<std::array<float,4>, 4> m;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct Line {
	Vector3 origine;
	Vector3 diff;
};

struct Ray {
	Vector3 origine;
	Vector3 diff;
};

struct Segment {
	Vector3 origine;
	Vector3 diff;
};

struct Plane {
	Vector3 normal;
	float distance;
};

struct Triangle {
	Vector3 vertixces[3];
};

struct AABB {
	Vector3 min;
	Vector3 max;
};

struct OBB {
	Vector3 center;
	Vector3 oriientations[3];
	Vector3 size;
};
