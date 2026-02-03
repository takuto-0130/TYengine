#include "ColliderManager.h"
#include "CollisionFunc.h"
#include "Sphere/SphereCollider.h"
#include "Ray/RayCollider.h"

void ColliderManager::Update()
{
    // 現在の衝突状態リスト
    std::unordered_set<ColliderPair, ColliderPairHash> currentCollisions;

    // 全ペア総当たり（ナイーブ実装）
    for (size_t i = 0; i < colliders_.size(); ++i) {
		for (size_t j = i + 1; j < colliders_.size(); ++j) {
			auto& a = *colliders_[i];
			auto& b = *colliders_[j];

			if (a.GetID() == 0 || b.GetID() == 0)
			{
                // ID未設定は無視
			}
			else if (CheckCollisionDispatcher(&a, &b)) {
				ColliderPair pair = MakeSortedPair(a.GetID(), b.GetID());
				currentCollisions.insert(pair);

				// 衝突情報生成
				CollisionInfo infoAtoB = GenerateInfo(a, b);
				CollisionInfo infoBtoA = infoAtoB;
				std::swap(infoBtoA.selfID, infoBtoA.otherID);
				infoBtoA.direction = -infoAtoB.direction;

                // 新規衝突ならEnter, 継続ならStay
				if (previousCollisions_.find(pair) == previousCollisions_.end()) {
					a.OnCollisionEnter(b, infoAtoB);
					b.OnCollisionEnter(a, infoBtoA);
					a.TriggerEnter(infoAtoB);
					b.TriggerEnter(infoBtoA);
				}
				else {
					a.OnCollisionStay(b, infoAtoB);
					b.OnCollisionStay(a, infoBtoA);
					a.TriggerStay(infoAtoB);
					b.TriggerStay(infoBtoA);
				}
			}
		}
    }

    // 衝突から外れたペアはExit
    for (const auto& pair : previousCollisions_) {
        if (currentCollisions.find(pair) == currentCollisions.end()) {
            auto* a = FindColliderByID(pair.first);
            auto* b = FindColliderByID(pair.second);
            if (a && b) {
                CollisionInfo infoAtoB = GenerateInfo(*a, *b);
                CollisionInfo infoBtoA = infoAtoB;
                std::swap(infoBtoA.selfID, infoBtoA.otherID);
                infoBtoA.direction = -infoAtoB.direction;

                a->OnCollisionExit(*b, infoAtoB);
                b->OnCollisionExit(*a, infoBtoA);
                a->TriggerExit(infoAtoB);
                b->TriggerExit(infoBtoA);
            }
        }
    }

    // 更新
    previousCollisions_ = std::move(currentCollisions);
}



bool ColliderManager::CheckCollisionDispatcher(Collider* a, Collider* b)
{
    if (!a || !b) {
        return false;
    }
    auto shapeA = a->GetShapeType();
    auto shapeB = b->GetShapeType();

    // enumの小さい順に並べて組み合わせの重複を避ける　例:(SPHERE→AABB　AABB→SPHERE 両方用意しなくていいように)
    if (shapeA > shapeB) {
        std::swap(a, b);
        std::swap(shapeA, shapeB);
    }

    if (shapeA == ColliderShape::SPHERE && shapeB == ColliderShape::SPHERE) {
        return IsCollision(
            static_cast<const SphereCollider*>(a)->GetSphere(),
            static_cast<const SphereCollider*>(b)->GetSphere());
    }

    if (shapeA == ColliderShape::SPHERE && shapeB == ColliderShape::RAY) {
        return IsCollision(
            static_cast<const SphereCollider*>(a)->GetSphere(),
            static_cast<const RayCollider*>(b)->GetRay());
    }


    return false;
}
