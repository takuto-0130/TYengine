#pragma once
#include "../../../BaseBullet/BaseBullet.h"
#include "StateMachine.h"
#include "State.h"
#include "../EBulletCollider.h"

namespace EnemyBullet
{
	// 前方宣言
	class LinearStateShot;
	class LinearStateAfterCollision;

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
		friend class LinearStateShot;
		friend class LinearStateAfterCollision;
	public:
		using StateMachineType = TYEngine::Utility::StateMachine<LinearState, Linear>;

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

	};

	// --- 状態クラスの定義 ---
	class LinearStateShot : public TYEngine::Utility::State<LinearState, Linear>
	{
	public:
		using State::State;
		void Init(Linear& owner) override;
		void Update(Linear& owner, float deltaTime) override;
		void Exit(Linear& owner) override;
	};

	class LinearStateAfterCollision : public TYEngine::Utility::State<LinearState, Linear>
	{
	public:
		using State::State;
		void Init(Linear& owner) override;
		void Update(Linear& owner, float deltaTime) override;
		void Exit(Linear& owner) override;
	};
}

