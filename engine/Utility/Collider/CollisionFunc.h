#pragma once
#include "struct.h"

bool IsCollision(const Sphere& sphere, const Plane& plane);

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

/// <summary>
/// 分離軸があるかどうか
/// </summary>
/// <param name="normal"> 正規化法線ベクトル </param>
/// <param name="vertixces1"> OBB1の頂点 </param>
/// <param name="vertixces2"> OBB2の頂点 </param>
/// <returns></returns>
bool isHST(const Vector3& normal, const Vector3* vertixces1, const Vector3* vertixces2);

bool IsCollision(const OBB& obb1, const OBB& obb2);
