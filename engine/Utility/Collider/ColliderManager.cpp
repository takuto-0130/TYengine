#include "ColliderManager.h"
#include "Sphere/SphereCollider.h"

void ColliderManager::Update()
{
    std::unordered_set<ColliderPair, ColliderPairHash> currentCollisions;

    for (size_t i = 0; i < colliders_.size(); ++i) {
		for (size_t j = i + 1; j < colliders_.size(); ++j) {
			auto& a = *colliders_[i];
			auto& b = *colliders_[j];

			if (a.GetID() == 0 || b.GetID() == 0)
			{

			}
			else if (CheckCollisionDispatcher(&a, &b)) {
				ColliderPair pair = MakeSortedPair(a.GetID(), b.GetID());
				currentCollisions.insert(pair);

				CollisionInfo infoAtoB = GenerateInfo(a, b);
				CollisionInfo infoBtoA = infoAtoB;
				std::swap(infoBtoA.selfID, infoBtoA.otherID);
				infoBtoA.direction = -infoAtoB.direction;

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

    previousCollisions_ = std::move(currentCollisions);
}




bool ColliderManager::CheckCollision(const SphereCollider& a, const SphereCollider& b) {
    float distSq = Length(a.GetCenter() - b.GetCenter());
    float rSum = a.GetRadius() + b.GetRadius();
    return distSq <= rSum;
}

//bool CheckCollision(const SphereCollider& a, const AABBCollider& b) {
//    // AABBとSphereの衝突処理
//}

bool ColliderManager::CheckCollisionDispatcher(Collider* a, Collider* b)
{
    if (!a || !b) {
        return false;
    }
    auto shapeA = a->GetShapeType();
    auto shapeB = b->GetShapeType();

    // 小さい順に並べて組み合わせの重複を避ける　例:(SPHERE→AABB　AABB→SPHERE 両方用意しなくていいように)
    if (shapeA > shapeB) {
        std::swap(a, b);
        std::swap(shapeA, shapeB);
    }

    if (shapeA == ColliderShape::SPHERE && shapeB == ColliderShape::SPHERE) {
        return CheckCollision(
            *static_cast<const SphereCollider*>(a),
            *static_cast<const SphereCollider*>(b));
    }

    /*if (shapeA == ColliderShape::SPHERE && shapeB == ColliderShape::AABB) {
        return CheckCollision(
            *static_cast<const SphereCollider*>(a),
            *static_cast<const AABBCollider*>(b));
    }*/


    return false;
}
