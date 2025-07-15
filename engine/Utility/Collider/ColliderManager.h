#pragma once
#include "Collider.h"
#include <vector>
#include <unordered_set>

using ColliderPair = std::pair<uint32_t, uint32_t>;

struct ColliderPairHash {
    std::size_t operator()(const ColliderPair& pair) const {
        return std::hash<uint32_t>()(pair.first) ^ std::hash<uint32_t>()(pair.second);
    }
};

class SphereCollider;
class ColliderManager {
public:
    static ColliderManager* GetInstance()
    {
        static ColliderManager instance;
        return &instance;
    }

    ~ColliderManager() = default;

    void AddCollider(Collider* collider) {
        colliders_.emplace_back(collider);
    }

    void RemoveCollider(Collider* collider) {
        auto it = std::remove(colliders_.begin(), colliders_.end(), collider);
        if (it != colliders_.end()) {
            colliders_.erase(it, colliders_.end());
        }
    }

    void Update();

public:

private:
    ColliderManager() = default;
    ColliderManager(ColliderManager&) = delete;
    ColliderManager& operator=(ColliderManager&) = delete;

    std::vector<Collider*> colliders_;
    std::unordered_set<ColliderPair, ColliderPairHash> previousCollisions_;

    ColliderPair MakeSortedPair(uint32_t a, uint32_t b) const {
        return (a < b) ? ColliderPair{ a, b } : ColliderPair{ b, a };
    }

    Collider* FindColliderByID(uint32_t id) {
        for (auto& c : colliders_) {
            if (c->GetID() == id) return c;
        }
        return nullptr;
    }

    /// <summary>
    /// 判定情報（衝突点の値や、中心座標がなかった場合の値は仮）
    /// </summary>
    CollisionInfo GenerateInfo(const Collider& a, const Collider& b) {
        if(a.GetCenter() && b.GetCenter())
        {
            Vector3 dir = a.GetCenter().value() - b.GetCenter().value();
            return CollisionInfo{
                a.GetID(),
                b.GetID(),
                (a.GetCenter().value() + b.GetCenter().value()) * 0.5f,
                Normalize(dir),
                Length(dir)
            };
        }
        else
		{
			return CollisionInfo{
				a.GetID(),
				b.GetID(),
				0,
				0,
				0
			};
		}
    }

    bool CheckCollisionDispatcher(Collider* a, Collider* b);
};