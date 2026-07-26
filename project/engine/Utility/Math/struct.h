#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <array>

namespace TYEngine
{
	namespace Utility
	{

		/// <summary>2次元ベクトル構造体。</summary>
		struct Vector2
		{
			float x;
			float y;

			Vector2& operator+=(const Vector2& a)
			{
				x = a.x + x;
				y = a.y + y;
				return *this;
			}

			Vector2& operator-=(const Vector2& a)
			{
				x = x - a.x;
				y = y - a.y;
				return *this;
			}

			Vector2& operator*=(float a)
			{
				x = x * a;
				y = y * a;
				return *this;
			}

			Vector2& operator/=(float a)
			{
				x = x / a;
				y = y / a;
				return *this;
			}

			bool operator==(const Vector2& a) const
			{
				return (x == a.x && y == a.y);
			}
		};

		/// <summary>3次元ベクトル構造体。</summary>
		struct Vector3
		{
			float x;
			float y;
			float z;

			Vector3& operator+=(const Vector3& a)
			{
				x = a.x + x;
				y = a.y + y;
				z = a.z + z;
				return *this;
			}

			Vector3& operator-=(const Vector3& a)
			{
				x = x - a.x;
				y = y - a.y;
				z = z - a.z;
				return *this;
			}

			Vector3& operator*=(float a)
			{
				x = x * a;
				y = y * a;
				z = z * a;
				return *this;
			}

			Vector3& operator/=(float a)
			{
				x = x / a;
				y = y / a;
				z = z / a;
				return *this;
			}

			bool operator==(const Vector3& a) const
			{
				return (x == a.x && y == a.y && z == a.z);
			}
		};

		/// <summary>4次元ベクトル（カラーRGBA・同次座標）構造体。</summary>
		struct Vector4
		{
			float x;
			float y;
			float z;
			float w;

			Vector4& operator+=(const Vector4& a)
			{
				x = a.x + x;
				y = a.y + y;
				z = a.z + z;
				w = a.w + w;
				return *this;
			}

			Vector4& operator-=(const Vector4& a)
			{
				x = x - a.x;
				y = y - a.y;
				z = z - a.z;
				w = w - a.w;
				return *this;
			}

			Vector4& operator*=(float a)
			{
				x = x * a;
				y = y * a;
				z = z * a;
				w = w * a;
				return *this;
			}

			Vector4& operator/=(float a)
			{
				x = x / a;
				y = y / a;
				z = z / a;
				w = w / a;
				return *this;
			}

			bool operator==(const Vector4& a) const
			{
				return (x == a.x && y == a.y && z == a.z && w == a.w);
			}
		};

		/// <summary>4x4 変換行列構造体。</summary>
		struct Matrix4x4
		{
			std::array<std::array<float, 4>, 4> m;
		};

		/// <summary>四元数（クォータニオン）回転構造体。</summary>
		struct Quaternion
		{
			float x;
			float y;
			float z;
			float w;
		};

		/// <summary>トランスフォーム（拡大・回転・移動）構造体。</summary>
		struct Transform
		{
			Vector3 scale;
			Vector3 rotate;
			Vector3 translate;
		};

		/// <summary>球体（Sphere）衝突境界構造体。</summary>
		struct Sphere
		{
			Vector3 center;
			float radius;
		};

		/// <summary>直線（Line）構造体。</summary>
		struct Line
		{
			Vector3 origin;
			Vector3 diff;
		};

		/// <summary>半直線（Ray）構造体。</summary>
		struct Ray
		{
			Vector3 origin;
			Vector3 diff;
		};

		/// <summary>線分（Segment）構造体。</summary>
		struct Segment
		{
			Vector3 origin;
			Vector3 diff;
		};

		/// <summary>平面（Plane）構造体。</summary>
		struct Plane
		{
			Vector3 normal;
			float distance;
		};

		/// <summary>三角形（Triangle）構造体。</summary>
		struct Triangle
		{
			Vector3 vertices[3];
		};

		/// <summary>軸並行境界ボックス（AABB）構造体。</summary>
		struct AABB
		{
			Vector3 min;
			Vector3 max;
		};

		/// <summary>有向境界ボックス（OBB）構造体。</summary>
		struct OBB
		{
			Vector3 center;
			Vector3 orientations[3];
			Vector3 size;
		};

	} // namespace Utility
} // namespace TYEngine
