#pragma once
#include <functional>
#include <optional>
#include <utility>
#include <cstdint>
#include "mathFunc.h"

enum class ColliderShape 
{
    SPHERE,
    LINE,
    RAY,
    SEGMENT,
    PLANE,
    TRIANGLE,
    AABB,
    OBB,
};

enum class CollisionState 
{
    ENTER,
    ON,
    EXIT
};

struct CollisionInfo 
{
    uint32_t selfID;
    uint32_t otherID;
    Vector3 contactPoint;
    Vector3 direction;
    float distance;
};

// コライダー基底クラス
class Collider 
{
public:
    using ID = uint32_t;
    using CollisionCallback = std::function<void(const CollisionInfo&)>;

    // コンストラクタ
    Collider(uint32_t typeID)
        : id_(GenerateID()), typeID_(typeID) {}
    // デストラクタ
    virtual ~Collider() = default;

    // 更新
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
    ID id_;
    uint32_t typeID_;
    CollisionCallback onEnter_, onStay_, onExit_;

    static ID GenerateID()
    {
        static ID nextID = 0;
        return nextID++;
    }
};

