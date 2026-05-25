#include "mathFunc.h"
#include "Matrix4x4Func.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <cassert>

namespace TYEngine
{
	namespace Utility
	{

		Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m)
		{
			Vector3 result{
				v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
				v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
				v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] };
			return result;
		}

		float Length(const Vector3& v)
		{
			return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		float Length(const Vector2& v)
		{
			return sqrtf(v.x * v.x + v.y * v.y);
		}

		Vector3 Normalize(const Vector3& v)
		{
			float len = Length(v);
			Vector3 result{ v.x / len, v.y / len, v.z / len };
			return result;
		}

		Vector2 Normalize(const Vector2& v)
		{
			float len = Length(v);
			Vector2 result{ v.x / len, v.y / len };
			return result;
		}

		float Dot(const Vector3& v1, const Vector3& v2)
		{
			return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
		}

		Vector3 Subtruct(const Vector3& v1, const Vector3& v2)
		{
			return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
		}

		Vector3 Add(const Vector3& v1, const Vector3& v2)
		{
			return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
		}

		const Vector3 Multiply(const Vector3& v1, const Vector3& v2)
		{
			return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
		}

		const Vector3 Multiply(const Vector3& v, float s)
		{
			return { v.x * s, v.y * s, v.z * s };
		}

		const Vector3 Multiply(float s, const Vector3& v)
		{
			return { v.x * s, v.y * s, v.z * s };
		}

		float Lerp(float a, float b, float t)
		{
			return a + t * (b - a);
		}

		Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t)
		{
			return v1 + t * (v2 - v1);
		}

		Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
		{
			return v1 + t * (v2 - v1);
		}

		Vector4 Lerp(const Vector4& v1, const Vector4& v2, float t)
		{
			return v1 + t * (v2 - v1);
		}

		Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t)
		{
			Vector3 nV1 = Normalize(v1);
			Vector3 nV2 = Normalize(v2);
			float dot = Dot(nV1, nV2);

			dot = (std::min)(dot, 1.0f);
			float theta = std::acos(dot);
			float sinTheta = std::sin(theta);
			float sinThetaFrom = std::sin((1 - t) * theta);
			float sinThetaTo = std::sin(t * theta);
			Vector3 nSlerp{};
			if (sinTheta < 1.0e-5)
			{
				nSlerp = nV1;
			}
			else
			{
				nSlerp = (sinThetaFrom * nV1 + sinThetaTo * nV2) / sinTheta;
			}
			float length = Lerp(Length(v1), Length(v2), t);

			return length * nSlerp;
		}


		// 2点間の距離に基づいて「時間(t)」を計算するヘルパー関数（アルファ=0.5 が求心性）
		static float GetCentripetalT(float t, const TYEngine::Utility::Vector3& p0, const TYEngine::Utility::Vector3& p1)
		{
			// 2点間の距離の平方（ルート計算を減らすため）
			float d2 = (p1.x - p0.x) * (p1.x - p0.x) +
				(p1.y - p0.y) * (p1.y - p0.y) +
				(p1.z - p0.z) * (p1.z - p0.z);

			// 距離の0.5乗 (d2の0.25乗 = ルートのルート)
			float a = std::sqrt(std::sqrt(d2));
			return t + a;
		}

		// ゼロ除算を回避しながら線形補間するヘルパー
		static TYEngine::Utility::Vector3 RemapLerp(float t_a, float t_b, float t_curr, const TYEngine::Utility::Vector3& p_a, const TYEngine::Utility::Vector3& p_b)
		{
			if (t_a == t_b) return p_a;
			float weight = (t_curr - t_a) / (t_b - t_a);
			return {
				p_a.x + (p_b.x - p_a.x) * weight,
				p_a.y + (p_b.y - p_a.y) * weight,
				p_a.z + (p_b.z - p_a.z) * weight
			};
		}

		TYEngine::Utility::Vector3 CatmullRomInterpolation(const TYEngine::Utility::Vector3& p0, const TYEngine::Utility::Vector3& p1, const TYEngine::Utility::Vector3& p2, const TYEngine::Utility::Vector3& p3, float t)
		{
			// 各制御点の「到達時間」を距離（の0.5乗）ベースで算出
			float t0 = 0.0f;
			float t1 = GetCentripetalT(t0, p0, p1);
			float t2 = GetCentripetalT(t1, p1, p2);
			float t3 = GetCentripetalT(t2, p2, p3);

			// p1 と p2 が全く同じ位置にある場合はそのまま返す
			if (t1 == t2) return p1;

			// 0.0～1.0 で渡される t を、実際の区間時間 (t1～t2) にマッピング
			float t_local = t1 + (t2 - t1) * t;

			// Barry-Goldmanのピラミッドアルゴリズムによる求心性補間
			TYEngine::Utility::Vector3 A1 = RemapLerp(t0, t1, t_local, p0, p1);
			TYEngine::Utility::Vector3 A2 = RemapLerp(t1, t2, t_local, p1, p2);
			TYEngine::Utility::Vector3 A3 = RemapLerp(t2, t3, t_local, p2, p3);

			TYEngine::Utility::Vector3 B1 = RemapLerp(t0, t2, t_local, A1, A2);
			TYEngine::Utility::Vector3 B2 = RemapLerp(t1, t3, t_local, A2, A3);

			TYEngine::Utility::Vector3 C = RemapLerp(t1, t2, t_local, B1, B2);

			return C;
		}

		// ★ CatmullRomPosition はご提示いただいた元のコードをそのまま使います！
		// （テンション引数などは削除して、元の状態に戻してOKです）
		TYEngine::Utility::Vector3 CatmullRomPosition(const std::vector<TYEngine::Utility::Vector3>& points, float t)
		{
			assert(points.size() >= 4 && "制御点は4点以上必要です");

			size_t division = points.size() - 1;

			// 全体の進行度 t (0.0～1.0) をセグメント数倍する (例: 0.0 ～ 4.0)
			float globalT = t * static_cast<float>(division);

			// 整数部をインデックス、小数部をローカル時間 t_2 として「同時に」切り出す
			size_t index = static_cast<size_t>(globalT);
			float t_2 = globalT - static_cast<float>(index);

			// t がちょうど 1.0f の時、または誤差で division 以上になった時の安全弁
			if (index >= division)
			{
				index = division - 1;
				t_2 = 1.0f;
			}

			// インデックスの前後関係を安全に割り振り
			size_t index1 = index;
			size_t index0 = (index1 == 0) ? index1 : index1 - 1;
			size_t index2 = index1 + 1;
			size_t index3 = index2 + 1;
			if (index3 >= points.size()) { index3 = index2; }

			const TYEngine::Utility::Vector3& p0 = points[index0];
			const TYEngine::Utility::Vector3& p1 = points[index1];
			const TYEngine::Utility::Vector3& p2 = points[index2];
			const TYEngine::Utility::Vector3& p3 = points[index3];

			// 先ほど作成した求心性（または通常版）の補間関数を呼び出す
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

		Vector3 Cross(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
		}

		void OBBRotation(const Vector3& rotate, OBB& obb)
		{
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





		Vector3 operator+(const Vector3& a, const Vector3& b)
		{
			Vector3 result;
			result.x = a.x + b.x;
			result.y = a.y + b.y;
			result.z = a.z + b.z;
			return result;
		}


		Vector3 operator-(const Vector3& a, const Vector3& b)
		{
			Vector3 result;
			result.x = a.x - b.x;
			result.y = a.y - b.y;
			result.z = a.z - b.z;
			return result;
		}

		Vector3 operator-(const Vector3& a)
		{
			Vector3 result;
			result.x = -a.x;
			result.y = -a.y;
			result.z = -a.z;
			return result;
		}


		Vector3 operator*(const Vector3& a, const Vector3& b)
		{
			Vector3 result;
			result.x = a.x * b.x;
			result.y = a.y * b.y;
			result.z = a.z * b.z;
			return result;
		}
		Vector3 operator*(const Vector3& v, float s)
		{
			Vector3 result;
			result.x = v.x * s;
			result.y = v.y * s;
			result.z = v.z * s;
			return result;
		}
		Vector3 operator*(float s, const Vector3& v)
		{
			Vector3 result;
			result.x = v.x * s;
			result.y = v.y * s;
			result.z = v.z * s;
			return result;
		}


		Vector3 operator/(const Vector3& a, const Vector3& b)
		{
			Vector3 result;
			result.x = a.x / b.x;
			result.y = a.y / b.y;
			result.z = a.z / b.z;
			return result;
		}
		Vector3 operator/(const Vector3& v, float s)
		{
			Vector3 result;
			result.x = v.x / s;
			result.y = v.y / s;
			result.z = v.z / s;
			return result;
		}

		Vector2 operator-(const Vector2& a, const Vector2& b)
		{
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


		Vector2 operator*(const Vector2& a, const Vector2& b)
		{
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


		Vector2 operator/(const Vector2& a, const Vector2& b)
		{
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
