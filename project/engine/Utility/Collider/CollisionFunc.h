#pragma once
#include "struct.h"

/// <summary>
/// 球と球の衝突判定を行う
/// </summary>
bool IsCollision(const Sphere& sphereA, const Sphere& sphereB);

/// <summary>
/// 球と平面の衝突判定を行う
/// </summary>
bool IsCollision(const Sphere& sphere, const Plane& plane);

/// <summary>
/// 球とレイの衝突判定を行う
/// </summary>
bool IsCollision(const Sphere& sphere, const Ray& ray);

/// <summary>
/// 直線と平面の衝突（交差）判定を行う
/// </summary>
bool IsCollision(const Line& line, const Plane& plane);

/// <summary>
/// 線分と平面の衝突（交差）判定を行う
/// </summary>
bool IsCollision(const Segment& segment, const Plane& plane);

/// <summary>
/// 線分と三角形の衝突（交差）判定を行う
/// </summary>
bool IsCollision(const Segment& segment, const Triangle& triangle);

/// <summary>
/// AABB（軸平行境界ボックス）同士の衝突判定を行う
/// </summary>
bool IsCollision(const AABB& a, const AABB& b);

/// <summary>
/// AABB と球の衝突判定を行う
/// </summary>
bool IsCollision(const AABB& a, const Sphere& sphere);

/// <summary>
/// AABB と直線の衝突判定を行う
/// </summary>
bool IsCollision(const AABB& a, const Line& line);

/// <summary>
/// AABB と線分の衝突判定を行う
/// </summary>
bool IsCollision(const AABB& a, const Segment& segment);

/// <summary>
/// OBB（方向付き境界ボックス）と球の衝突判定を行う
/// </summary>
bool IsCollision(const OBB& obb, const Sphere& sphere);

/// <summary>
/// OBB と線分の衝突判定を行う
/// </summary>
bool IsCollision(const OBB& obb, const Segment& segment);

/// <summary>
/// OBB の8頂点を算出する
/// </summary>
void OBBVertex(const OBB& obb, Vector3* vertices);

/// <summary>
/// 分離軸（Separating Axis Theorem, SAT）に基づき、2つの形状が投影上で重なっているかを判定する
/// </summary>
bool isHST(const Vector3& normal, const Vector3* vertices1, const Vector3* vertices2);

/// <summary>
/// OBB 同士の衝突判定を行う
/// </summary>
bool IsCollision(const OBB& obb1, const OBB& obb2);

