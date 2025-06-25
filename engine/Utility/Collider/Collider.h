#pragma once

#include <functional>
#include <utility>
#include <cstdint>
#include "mathFunc.h"

enum class CollisionState {
    ENTER,
    ON,
    EXIT
};

struct CollisionInfo {
    uint32_t selfID;
    uint32_t otherID;
    Vector3 contactPoint;
    Vector3 direction;
    float distance;
};

class Collider;

using ColliderPair = std::pair<uint32_t, uint32_t>;

struct ColliderPairHash {
    std::size_t operator()(const ColliderPair& pair) const {
        return std::hash<uint32_t>()(pair.first) ^ std::hash<uint32_t>()(pair.second);
    }
};

class Collider {
public:
    using ID = uint32_t;
    using CollisionCallback = std::function<void(const CollisionInfo&)>;

    Collider(uint32_t typeID)
        : id_(GenerateID()), typeID_(typeID) {
    }

    virtual ~Collider() = default;

    ID GetID() const { return id_; }
    uint32_t GetTypeID() const { return typeID_; }

    virtual Vector3 GetCenter() const = 0;
    virtual void Update(const Vector3& pos) = 0;
    virtual bool CheckCollision(const Collider& other) const = 0;
    virtual bool CheckCollisionWith(const class SphereCollider& other) const = 0;

    virtual void OnCollisionEnter([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) {}
    virtual void OnCollisionStay([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) {}
    virtual void OnCollisionExit([[maybe_unused]] Collider& other, [[maybe_unused]] const CollisionInfo& info) {}

    void SetOnEnter(CollisionCallback cb) { onEnter_ = std::move(cb); }
    void SetOnStay(CollisionCallback cb) { onStay_ = std::move(cb); }
    void SetOnExit(CollisionCallback cb) { onExit_ = std::move(cb); }

    void TriggerEnter(const CollisionInfo& info) { if (onEnter_) onEnter_(info); }
    void TriggerStay(const CollisionInfo& info) { if (onStay_) onStay_(info); }
    void TriggerExit(const CollisionInfo& info) { if (onExit_) onExit_(info); }

private:
    ID id_;
    uint32_t typeID_;
    CollisionCallback onEnter_, onStay_, onExit_;

    static ID GenerateID() {
        static ID nextID = 0;
        return nextID++;
    }
};

