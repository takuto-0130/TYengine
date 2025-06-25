#include "ColliderManager.h"

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
			else if (a.CheckCollision(b)) {
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