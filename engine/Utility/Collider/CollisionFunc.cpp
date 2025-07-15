#include "CollisionFunc.h"
#include "mathFunc.h"
#include "Matrix4X4Func.h"
#include <cmath>
#include <algorithm>

bool IsCollision(const Sphere& sphereA, const Sphere& sphereB)
{
	float distance = Length(sphereA.center - sphereB.center);
	return distance <= sphereA.radius + sphereB.radius;
}

bool IsCollision(const Sphere& sphere, const Plane& plane)
{
	float distance = sqrtf((Dot(plane.normal, sphere.center) - plane.distance) * (Dot(plane.normal, sphere.center) - plane.distance));
	return distance <= sphere.radius;
}

bool IsCollision(const Sphere& sphere, const Ray& ray)
{
	Vector3 m = ray.origin - sphere.center;
	float b = Dot(m, ray.diff);
	float c = Dot(m, m) - sphere.radius * sphere.radius;

	if (c > 0.0f && b > 0.0f) 
	{
		return false;
	}

	float discriminant = b * b - c;
	if (discriminant < 0.0f) 
	{
		return false;
	}

	return true;
}

bool IsCollision(const Line& line, const Plane& plane) 
{
	float dot = Dot(line.diff, plane.normal);
	return dot != 0;
}

bool IsCollision(const Segment& segment, const Plane& plane)
{
	float dot = Dot(segment.diff, plane.normal);
	if (dot == 0) 
	{
		return false;
	}
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;
	if (t < 0 || t > 1) 
	{
		return false;
	}
	return true;
}

bool IsCollision(const Segment& segment, const Triangle& triangle) 
{
	Vector3 v01 = triangle.vertixces[1] - triangle.vertixces[0];
	Vector3 v12 = triangle.vertixces[2] - triangle.vertixces[1];
	Vector3 v20 = triangle.vertixces[0] - triangle.vertixces[2];

	Vector3 normal = Cross(v01, v12);
	float distance = Dot(triangle.vertixces[0], normal);
	Plane plane = 
	{
		normal,
		distance
	};
	if (!(IsCollision(segment, plane))) 
	{
		return false;
	}
	float dot = Dot(segment.diff, plane.normal);
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;
	Vector3 point = ((segment.diff) * t) + segment.origin;

	Vector3 v1p = point - triangle.vertixces[1];
	Vector3 v2p = point - triangle.vertixces[2];
	Vector3 v0p = point - triangle.vertixces[0];

	Vector3 corss01 = Cross(v01, v1p);
	Vector3 corss12 = Cross(v12, v2p);
	Vector3 corss20 = Cross(v20, v0p);

	if (Dot(corss01, normal) >= 0.0f &&
		Dot(corss12, normal) >= 0.0f &&
		Dot(corss20, normal) >= 0.0f) 
	{
		return true;
	}
	return false;
}

bool IsCollision(const AABB& a, const AABB& b) 
{
	if ((a.min.x <= b.max.x && a.max.x >= b.min.x) &&
		(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
		(a.min.z <= b.max.z && a.max.z >= b.min.z)) 
	{
		return true;
	}
	return false;
}

bool IsCollision(const AABB& a, const Sphere& sphere)
{
	Vector3 closestPoint = { std::clamp(sphere.center.x,a.min.x,a.max.x), std::clamp(sphere.center.y,a.min.y,a.max.y), std::clamp(sphere.center.z,a.min.z,a.max.z) };
	float distance = Length(closestPoint - sphere.center);
	return distance <= sphere.radius;
}

bool IsCollision(const AABB& a, const Line& line)
{
	if (line.diff.x == 0.0f && line.diff.y == 0.0f && line.diff.z == 0.0f)
	{
		return false;
	}
	//X軸
	float tMinX = (a.min.x - line.origin.x) / line.diff.x;

	float tMaxX = (a.max.x - line.origin.x) / line.diff.x;

	float tNearX = std::min(tMinX, tMaxX);
	float tFarX = std::max(tMinX, tMaxX);


	//Y軸
	float tMinY = (a.min.y - line.origin.y) / line.diff.y;

	float tMaxY = (a.max.y - line.origin.y) / line.diff.y;

	float tNearY = std::min(tMinY, tMaxY);
	float tFarY = std::max(tMinY, tMaxY);


	//Z軸
	float tMinZ = (a.min.z - line.origin.z) / line.diff.z;

	float tMaxZ = (a.max.z - line.origin.z) / line.diff.z;

	float tNearZ = std::min(tMinZ, tMaxZ);
	float tFarZ = std::max(tMinZ, tMaxZ);


	float tMin = std::max(std::max(tNearX, tNearY), tNearZ);

	float tMax = std::min(std::min(tFarX, tFarY), tFarZ);

	
	return tMin <= tMax;
}


bool IsCollision(const AABB& a, const Segment& segment)
{
	if (segment.diff.x == 0.0f && segment.diff.y == 0.0f && segment.diff.z == 0.0f)
	{
		return false;
	}
	//X軸
	float tMinX = (a.min.x - segment.origin.x) / segment.diff.x;

	float tMaxX = (a.max.x - segment.origin.x) / segment.diff.x;

	float tNearX = std::min(tMinX, tMaxX);
	float tFarX = std::max(tMinX, tMaxX);


	//Y軸
	float tMinY = (a.min.y - segment.origin.y) / segment.diff.y;

	float tMaxY = (a.max.y - segment.origin.y) / segment.diff.y;

	float tNearY = std::min(tMinY, tMaxY);
	float tFarY = std::max(tMinY, tMaxY);


	//Z軸
	float tMinZ = (a.min.z - segment.origin.z) / segment.diff.z;

	float tMaxZ = (a.max.z - segment.origin.z) / segment.diff.z;

	float tNearZ = std::min(tMinZ, tMaxZ);
	float tFarZ = std::max(tMinZ, tMaxZ);


	float tMin = std::max(std::max(tNearX, tNearY), tNearZ);

	float tMax = std::min(std::min(tFarX, tFarY), tFarZ);

	if (tMin > tMax)
	{
		return false;
	}
	if ((tMin >= 0.0f && tMin <= 1.0f) || (tMax >= 0.0f && tMax <= 1.0f) ||
		(tMin <= 0.0f && tMax >= 1.0f)) 
	{
		return true;
	}


	return false;
}

bool IsCollision(const OBB& obb, const Sphere& sphere)
{
	Matrix4x4 obbMat = {};
	obbMat.m[0][0] = obb.oriientations[0].x;
	obbMat.m[0][1] = obb.oriientations[0].y;
	obbMat.m[0][2] = obb.oriientations[0].z;

	obbMat.m[1][0] = obb.oriientations[1].x;
	obbMat.m[1][1] = obb.oriientations[1].y;
	obbMat.m[1][2] = obb.oriientations[1].z;

	obbMat.m[2][0] = obb.oriientations[2].x;
	obbMat.m[2][1] = obb.oriientations[2].y;
	obbMat.m[2][2] = obb.oriientations[2].z;

	obbMat.m[3][0] = obb.center.x;
	obbMat.m[3][1] = obb.center.y;
	obbMat.m[3][2] = obb.center.z;

	obbMat.m[3][3] = 1.0f;

	Matrix4x4 obbMatInverse = Inverse(obbMat);
	Vector3 centerInOBBLocalSpace = TransformM(sphere.center, obbMatInverse);

	AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
	Sphere sphereOBBLocal{ centerInOBBLocalSpace, sphere.radius };
	return IsCollision(aabbOBBLocal, sphereOBBLocal);
}

bool IsCollision(const OBB& obb, const Segment& segment) 
{
	Matrix4x4 obbMat = {};
	obbMat.m[0][0] = obb.oriientations[0].x;
	obbMat.m[0][1] = obb.oriientations[0].y;
	obbMat.m[0][2] = obb.oriientations[0].z;

	obbMat.m[1][0] = obb.oriientations[1].x;
	obbMat.m[1][1] = obb.oriientations[1].y;
	obbMat.m[1][2] = obb.oriientations[1].z;

	obbMat.m[2][0] = obb.oriientations[2].x;
	obbMat.m[2][1] = obb.oriientations[2].y;
	obbMat.m[2][2] = obb.oriientations[2].z;

	obbMat.m[3][0] = obb.center.x;
	obbMat.m[3][1] = obb.center.y;
	obbMat.m[3][2] = obb.center.z;

	obbMat.m[3][3] = 1.0f;

	Matrix4x4 obbMatInverse = Inverse(obbMat);
	Vector3 localOrigin = TransformM(segment.origin, obbMatInverse);
	Vector3 localEnd = TransformM(segment.origin + segment.diff, obbMatInverse);

	AABB aabbOBBLocal{ .min = -obb.size, .max = obb.size };
	Segment localSegment{ .origin{localOrigin}, .diff{localEnd - localOrigin} };
	return IsCollision(aabbOBBLocal, localSegment);
}

void OBBVertex(const OBB& obb, Vector3* vertixces) 
{
	Vector3 sizeX = (obb.oriientations[0]) * obb.size.x;
	Vector3 sizeY = (obb.oriientations[1]) * obb.size.y;
	Vector3 sizeZ = (obb.oriientations[2]) * obb.size.z;
	vertixces[0] = -sizeX - sizeY - sizeZ + obb.center;
	vertixces[1] = -sizeX + sizeY - sizeZ + obb.center;
	vertixces[2] = sizeX - sizeY - sizeZ + obb.center;
	vertixces[3] = sizeX + sizeY - sizeZ + obb.center;
	vertixces[4] = -sizeX - sizeY + sizeZ + obb.center;
	vertixces[5] = -sizeX + sizeY + sizeZ + obb.center;
	vertixces[6] = sizeX - sizeY + sizeZ + obb.center;
	vertixces[7] = sizeX + sizeY + sizeZ + obb.center;
}

bool isHST(const Vector3& normal, const  Vector3* vertixces1, const  Vector3* vertixces2) 
{
	float t[8];
	for (size_t i = 0; i < 8; i++) 
	{
		t[i] = Dot(normal, vertixces1[i]);
	}
	float minT1 = t[0], maxT1 = t[0];
	for (size_t i = 1; i < 8; i++)
	{
		minT1 = (std::min)(t[i], minT1);
		maxT1 = (std::max)(t[i], maxT1);
	}
	float L1 = maxT1 - minT1;

	for (size_t i = 0; i < 8; i++)
	{
		t[i] = Dot(normal, vertixces2[i]);
	}
	float minT2 = t[0], maxT2 = t[0];
	for (size_t i = 1; i < 8; i++) 
	{
		minT2 = (std::min)(t[i], minT2);
		maxT2 = (std::max)(t[i], maxT2);
	}
	float L2 = maxT2 - minT2;
	float sumSpan = L1 + L2;
	float longSpan = (std::max)(maxT1, maxT2) - (std::min)(minT1, minT2);

	return sumSpan < longSpan;
}

bool IsCollision(const OBB& obb1, const OBB& obb2)
{
	Vector3 obb1Vertices[8]{};
	OBBVertex(obb1, obb1Vertices);
	Vector3 obb2Vertices[8]{};
	OBBVertex(obb2, obb2Vertices);
	for (size_t i = 0; i < 3; i++) 
	{
		if (isHST(obb1.oriientations[i], obb1Vertices, obb2Vertices))
		{
			return false;
		}
		if (isHST(obb2.oriientations[i], obb1Vertices, obb2Vertices)) 
		{
			return false;
		}
	}
	Vector3 cross{};
	for (size_t i = 0; i < 3; i++) 
	{
		for (size_t j = 0; j < 3; j++) 
		{
			cross = Cross(obb1.oriientations[i], obb2.oriientations[j]);
			cross = Normalize(cross);
			if (isHST(cross, obb1Vertices, obb2Vertices)) 
			{
				return false;
			}
		}
	}
	return true;
}
