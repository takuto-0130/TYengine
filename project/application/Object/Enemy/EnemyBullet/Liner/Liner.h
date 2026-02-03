#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"
#include "../EBulletCollider.h"

namespace EnemyBullet
{
	/// <summary>
	/// 直線弾の状態定数。
	/// </summary>
	enum class LinerState
	{
		SHOT,            ///< 発射・移動中
		AFTER_COLLISION, ///< 衝突後（着弾）
	};

	/// <summary>
	/// 直線軌道の敵弾クラス。
	/// 発射ステートと衝突後ステートを持つ。
	/// </summary>
	class Liner :
		public BaseBullet, StateMachine<Liner, LinerState>
	{
	public: // 関数テーブル
		static const std::vector<StateFunctionSet>& GetStateTable();

	public:
		Liner();
		~Liner();
		void Init()override;
		void Update()override;
		void Draw()override;

	private:
		void Move();
		void RotationDirection();

	private:
		std::unique_ptr<EBulletCollider> collider_;

		float deltaTime_ = 1.0f / 60.0f;


	private: // シーン内のState関連関数
#pragma region // State関連関数
		// 列挙名を文字列化（ImGui表示用）
		std::string GetStateName(State state) const override
		{
			switch (state)
			{
			case State::SHOT: return "SHOT";
			case State::AFTER_COLLISION: return "AFTER_COLLISION";
			default: return "Unknown";
			}
		}

		// 直線移動
		// ShotState.cpp
		void InitShot();
		void UpdateShot();
		void ExitShot();

		// 衝突後処理
		// AfterCollisionState.cpp
		void InitAfterCollision();
		void UpdateAfterCollision();
		void ExitAfterCollision();
#pragma endregion
	};
}

