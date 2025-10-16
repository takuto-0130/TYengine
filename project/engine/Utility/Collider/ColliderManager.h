#pragma once
#include "Collider.h"
#include <vector>
#include <unordered_set>

using ColliderPair = std::pair<uint32_t, uint32_t>;

struct ColliderPairHash 
{
    std::size_t operator()(const ColliderPair& pair) const
    {
        return std::hash<uint32_t>()(pair.first) ^ std::hash<uint32_t>()(pair.second);
    }
};

class SphereCollider;

/// <summary>
/// 当たり判定の管理クラス
/// </summary>
class ColliderManager 
{
public: // メンバ関数
    // シングルトンインスタンス
    static ColliderManager* GetInstance()
    {
        static ColliderManager instance;
        return &instance;
    }
    // デストラクタ
    ~ColliderManager() = default;

    /// <summary>
    /// コライダーを追加
    /// </summary>
    /// <param name="collider"> 登録したいコライダー</param>
    void AddCollider(Collider* collider)
    {
        colliders_.emplace_back(collider);
    }

    /// <summary>
    /// コライダーを削除
    /// </summary>
    /// <param name="collider"> 削除したいコライダー</param>
    void RemoveCollider(Collider* collider)
    {
        auto it = std::remove(colliders_.begin(), colliders_.end(), collider);
        if (it != colliders_.end()) 
        {
            colliders_.erase(it, colliders_.end());
        }
    }

    // 更新
    void Update();

private: // メンバ関数
    // コンストラクタ
    ColliderManager() = default;

    // コピー・ムーブの禁止
    ColliderManager(const ColliderManager&) = delete;
    ColliderManager& operator=(const ColliderManager&) = delete;
    ColliderManager(ColliderManager&&) = delete;
    ColliderManager& operator=(ColliderManager&&) = delete;

    /// <summary>
    /// 2つのコライダーIDをソートしてペアを作成する  
    /// （順序に依存しない衝突ペア判定のため）
    /// </summary>
    ColliderPair MakeSortedPair(uint32_t a, uint32_t b) const
    {
        return (a < b) ? ColliderPair{ a, b } : ColliderPair{ b, a };
    }

    /// <summary>
    /// IDからコライダーを検索する
    /// </summary>
    Collider* FindColliderByID(uint32_t id)
    {
        for (auto& c : colliders_)
        {
            if (c->GetID() == id) return c;
        }
        return nullptr;
    }

    /// <summary>
    /// 判定情報（衝突点の値や、中心座標がなかった場合の値は仮）
    /// </summary>
    CollisionInfo GenerateInfo(const Collider& a, const Collider& b)
    {
        if(a.GetCenter() && b.GetCenter())
        {
            Vector3 dir = a.GetCenter().value() - b.GetCenter().value();
            return CollisionInfo
            {
                a.GetID(),
                b.GetID(),
                (a.GetCenter().value() + b.GetCenter().value()) * 0.5f,
                Normalize(dir),
                Length(dir)
            };
        }
        else
		{
			return CollisionInfo
            {
				a.GetID(),
				b.GetID(),
				0,
				0,
				0
			};
		}
    }

    /// <summary>
    /// 2つのコライダー間の衝突をディスパッチ（判定）する  
    /// それぞれの形状タイプに応じた適切な判定関数を呼び出す
    /// </summary>
    bool CheckCollisionDispatcher(Collider* a, Collider* b);

private: // メンバ変数
    std::vector<Collider*> colliders_;
    std::unordered_set<ColliderPair, ColliderPairHash> previousCollisions_;
};