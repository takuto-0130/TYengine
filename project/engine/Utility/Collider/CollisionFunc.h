#pragma once
#include "struct.h"

bool IsCollision(const Sphere& sphereA, const Sphere& sphereB);

bool IsCollision(const Sphere& sphere, const Plane& plane);

bool IsCollision(const Sphere& sphere, const Ray& ray);

bool IsCollision(const Line& line, const Plane& plane);

bool IsCollision(const Segment& segment, const Plane& plane);

bool IsCollision(const Segment& segment, const Triangle& triangle);

bool IsCollision(const AABB& a, const AABB& b);

bool IsCollision(const AABB& a, const Sphere& sphere);

bool IsCollision(const AABB& a, const Line& line);

bool IsCollision(const AABB& a, const Segment& segment);

bool IsCollision(const OBB& obb, const Sphere& sphere);

bool IsCollision(const OBB& obb, const Segment& segment);

void OBBVertex(const OBB& obb, Vector3* vertixces);

bool isHST(const Vector3& normal, const Vector3* vertixces1, const Vector3* vertixces2);

bool IsCollision(const OBB& obb1, const OBB& obb2);
