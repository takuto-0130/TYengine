#pragma once
#include <functional>
#include <optional>
#include <utility>
#include <cstdint>
#include "MathFunc.h"

namespace TYEngine
{
	namespace Utility
	{


		/// <summary>
		/// コライダーの形状タイプ定義
		/// </summary>
		enum class ColliderShape
		{
			SPHERE,     ///< 球
			LINE,       ///< 直線
			RAY,        ///< レイ
			SEGMENT,    ///< 線分
			PLANE,      ///< 平面
			TRIANGLE,   ///< 三角形
			AABB,       ///< AABB
			OBB,        ///< OBB
		};

		/// <summary>
		/// 衝突状態の定義
		/// </summary>
		enum class CollisionState
		{
			ENTER,  ///< 接触開始
			ON,     ///< 接触中
			EXIT    ///< 接触終了
		};

		/// <summary>
		/// 衝突情報を保持する構造体
		/// </summary>
		struct CollisionInfo
		{
			uint32_t selfID;        ///< 自身のID
			uint32_t otherID;       ///< 相手のID
			Vector3 contactPoint;   ///< 接点
			Vector3 direction;      ///< 押し出し方向など
			float distance;         ///< 距離
		};

		/// <summary>
		/// コライダーの基底クラス。
		/// 形状に関わらず共通のID管理やコールバック機能を提供する。
		/// </summary>
		class Collider
		{
		public:
			using ID = uint32_t;
			using CollisionCallback = std::function<void(const CollisionInfo&)>;

			/// <summary>コンストラクタ。</summary>
			Collider(uint32_t typeID)
				: id_(GenerateID()), typeID_(typeID)
			{}
			/// <summary>デストラクタ。</summary>
			virtual ~Collider() = default;

			/// <summary>
			/// 更新処理。
			/// オブジェクトの位置に追従させる場合などに使用する。
			/// </summary>
			/// <param name="pos">現在の位置。</param>
			virtual void Update(const Vector3& pos) = 0;

			// getter / setter
			ID GetID() const { return id_; }
			void SetTypeID(uint32_t typeID) { typeID_ = typeID; }
			uint32_t GetTypeID() const { return typeID_; }
			virtual ColliderShape GetShapeType() const = 0;
			virtual std::optional<Vector3> GetCenter() const { return std::nullopt; }
			void SetOnEnter(CollisionCallback cb) { onEnter_ = std::move(cb); }
			void SetOnStay(CollisionCallback cb) { onStay_ = std::move(cb); }
			void SetOnExit(CollisionCallback cb) { onExit_ = std::move(cb); }

			// 衝突処理
			virtual void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) {}
			virtual void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) {}
			virtual void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) {}

			// 衝突時処理
			void TriggerEnter(const CollisionInfo& info) { if (onEnter_) onEnter_(info); }
			void TriggerStay(const CollisionInfo& info) { if (onStay_) onStay_(info); }
			void TriggerExit(const CollisionInfo& info) { if (onExit_) onExit_(info); }

		private:
			/// <summary>コライダーのユニークID。</summary>
			ID id_;
			/// <summary>オブジェクト種別ID（タグなど）。</summary>
			uint32_t typeID_;
			/// <summary>衝突コールバック（開始、維持、終了）。</summary>
			CollisionCallback onEnter_, onStay_, onExit_;

			/// <summary>ユニークIDを生成する。</summary>
			static ID GenerateID()
			{
				static ID nextID = 0;
				return nextID++;
			}
		};

	} // namespace Utility
} // namespace TYEngine

