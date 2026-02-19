#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"
#include "../EBulletCollider.h"

namespace EnemyBullet
{
	/// <summary>
	/// 直線弾の状態定数。
	/// </summary>
	enum class LinearState
	{
		SHOT,            ///< 発射・移動中
		AFTER_COLLISION, ///< 衝突後（着弾）
	};

	/// <summary>
	/// 直線軌道の敵弾クラス。
	/// 発射ステートと衝突後ステートを持つ。
	/// </summary>
	class Linear :
		public BaseBullet
	{
	public:
		using StateMachineType = TYEngine::Utility::StateMachine<Linear, LinearState>;
		using StateFunctionSet = StateMachineType::StateFunctionSet;
		// 関数テーブル
		static const std::vector<StateFunctionSet>& GetStateTable();

	public:
		Linear();
		~Linear();
		void Init()override;
		void Update()override;
		void Draw()override;

	private:
		/// <summary>移動処理（直進）。</summary>
		void Move();
		/// <summary>進行方向に合わせた回転更新。</summary>
		void RotationDirection();

	private:
		/// <summary>敵弾用コライダー。</summary>
		std::unique_ptr<EBulletCollider> collider_;

		/// <summary>固定タイムステップ。</summary>
		float deltaTime_ = 0.0f;

		/// <summary>ステートマシーン。</summary>
		StateMachineType stateMachine_;

	private: // シーン内のState関連関数
#pragma region // State関連関数
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

