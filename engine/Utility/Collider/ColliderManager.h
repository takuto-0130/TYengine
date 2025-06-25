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
        colliders_.erase(
            std::remove(colliders_.begin(), colliders_.end(), collider),
            colliders_.end()
        );
    }

    void Update();

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

    CollisionInfo GenerateInfo(const Collider& a, const Collider& b) {
        Vector3 dir = a.GetCenter() - b.GetCenter();
        return CollisionInfo{
            a.GetID(),
            b.GetID(),
            (a.GetCenter() + b.GetCenter()) * 0.5f,
            Normalize(dir),
            Length(dir)
        };
    }
};