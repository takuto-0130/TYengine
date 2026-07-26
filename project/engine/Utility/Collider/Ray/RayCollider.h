#pragma once
#include "Collider.h"
#include "struct.h"

namespace TYEngine
{
	namespace Utility
	{


		/// <summary>
		/// 直線コライダークラス
		/// </summary>
		class RayCollider :
			public Collider
		{
		public: // メンバ関数
			// コンストラクタ
			RayCollider(uint32_t typeID, const Vector3& origin, const Vector3& dir)
				: Collider(typeID), ray_(Ray{ origin,dir })
			{}

			// getter / setter
			/// <summary>形状タイプを取得する。</summary>
			ColliderShape GetShapeType() const override { return ColliderShape::RAY; }
			/// <summary>起点座標を取得する。</summary>
			std::optional<Vector3> GetCenter() const override { return ray_.origin; }
			/// <summary>方向ベクトルを取得する（長さを持つ場合あり）。</summary>
			Vector3 GetDiff() const { return ray_.diff; }
			/// <summary>レイ情報を取得する。</summary>
			Ray GetRay() const { return ray_; }
			/// <summary>方向ベクトルを設定する。</summary>
			void SetDirection(const Vector3& dir) { ray_.diff = dir; }

			/// <summary>
			/// 座標を更新する。
			/// </summary>
			/// <param name="pos">新しい起点座標。</param>
			void Update(const Vector3& pos) override { ray_.origin = pos; }

		private: // メンバ変数
			Ray ray_;
		};

	} // namespace Utility
} // namespace TYEngine

