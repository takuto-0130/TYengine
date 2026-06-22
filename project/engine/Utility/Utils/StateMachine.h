#pragma once
#include <unordered_map>
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include <cassert>
#include <type_traits>
#include "State.h"
#include <imgui.h>

namespace TYEngine
{
	namespace Utility
	{
		/// <summary>
		/// 汎用ステートマシンクラス（テンプレート）。
		/// 各ステートをクラス(State継承クラス)として定義し、インスタンスで管理する。
		/// </summary>
		/// <typeparam name="StateEnum">ステートを定義するEnum型。</typeparam>
		/// <typeparam name="Owner">ステートマシンを所有するクラス(Player, Enemy等)。</typeparam>
		template<typename StateEnum, typename Owner>
		class StateMachine
		{
		public:
			using StateType = State<StateEnum, Owner>;

		public: // メンバ関数

			/// <summary>
			/// ステートの登録。
			/// ステートクラスを内部で生成し、管理する。
			/// </summary>
			template<class T, class... Args>
			void RegisterState(
				StateEnum key,
				std::string name = "",
				Args&&... args)
			{
				static_assert(std::is_base_of_v<StateType, T>, "T must inherit from StateType");

				// 同じステートの重複登録禁止
				assert(!stateMap_.contains(key) && "State already registered.");

				if (name.empty())
				{
					name = "State_" + std::to_string(
						static_cast<int>(key));
				}

				stateNames_[key] = name;

				stateMap_.emplace(key, std::make_unique<T>(this, std::forward<Args>(args)...));

				stateList_.push_back(key);
			}

			/// <summary>
			/// ステート変更リクエスト。
			/// 実際の切り替えはUpdateState内で行われる。
			/// </summary>
			void ChangeState(StateEnum next)
			{
				// 未登録ステート禁止
				assert(stateMap_.contains(next) && "State is not registered.");

				// 同一ステート連続リクエスト防止
				if (stateRequest_ != next)
				{
					stateRequest_ = next;
				}
			}

			/// <summary>
			/// ステート更新処理。
			/// Exit -> Init の順で安全に切り替えを行う。
			/// </summary>
			void UpdateState(Owner& owner, float deltaTime)
			{
				// 遷移リクエスト処理
				while (stateRequest_ && CanExitState())
				{
					ApplyStateChange(owner);
				}

				// 現在のステート更新
				if (currStatePtr_)
				{
					// Update中の状態変更対策
					StateType* current = currStatePtr_;

					current->TimerUpdate(deltaTime);
					current->Update(owner, deltaTime);
				}
			}

			/// <summary>
			/// デバッグ用ImGui表示
			/// </summary>
			void DebugImGui(const char* labelPrefix = "StateMachine")
			{
				std::string currentName = "None";

				if (currState_ && stateNames_.contains(*currState_))
				{
					currentName = stateNames_.at(*currState_);
				}

				ImGui::Text("%s: %s", labelPrefix, currentName.c_str());

				ImGui::Text("Elapsed: %.2f sec", GetStateElapsedTime());

				// ステート切り替えCombo
				if (!stateList_.empty())
				{
					if (ImGui::BeginCombo("Change State", currentName.c_str()))
					{
						for (auto key : stateList_)
						{
							bool isSelected = (currState_ && *currState_ == key);

							if (ImGui::Selectable(stateNames_[key].c_str(), isSelected))
							{
								ChangeState(key);
							}
						}

						ImGui::EndCombo();
					}
				}

				ImGui::Text("LockCount: %u", lockCount_);
			}

			/// <summary>
			/// 現在のステート取得
			/// </summary>
			std::optional<StateEnum> GetCurrentState() const { return currState_; }

			/// <summary>
			/// 前回のステート取得
			/// </summary>
			std::optional<StateEnum> GetPreviousState() const { return prevState_; }

			/// <summary>
			/// 現在ステートの経過時間取得
			/// </summary>
			float GetStateElapsedTime() const
			{
				return currStatePtr_ ? currStatePtr_->GetElapsed() : 0.0f;
			}

			/// <summary>
			/// ステート遷移ロック
			/// </summary>
			void LockState() { ++lockCount_; }

			/// <summary>
			/// ステート遷移ロック解除
			/// </summary>
			void UnlockState()
			{
				if (lockCount_ > 0)
				{
					--lockCount_;
				}
			}

			/// <summary>
			/// ステート遷移可能か
			/// </summary>
			bool CanExitState() const { return lockCount_ == 0; }

		private:

			/// <summary>
			/// ステート切り替え本体
			/// </summary>
			void ApplyStateChange(Owner& owner)
			{
				assert(stateRequest_.has_value());

				StateEnum next = *stateRequest_;

				// リクエスト消費
				stateRequest_.reset();

				// 現在ステート終了
				if (currStatePtr_)
				{
					currStatePtr_->Exit(owner);
				}

				// ステート更新
				prevState_ = currState_;
				currState_ = next;

				// ステート取得
				currStatePtr_ = stateMap_.at(next).get();

				assert(currStatePtr_ && "State pointer is null.");

				// タイマー初期化
				currStatePtr_->stateTimer_ = 0.0f;

				// 初期化
				currStatePtr_->Init(owner);
			}

		private:
			/// <summary>
			/// 現在のステート
			/// </summary>
			std::optional<StateEnum> currState_;

			/// <summary>
			/// 前回のステート
			/// </summary>
			std::optional<StateEnum> prevState_;

			/// <summary>
			/// 現在のステートクラス
			/// </summary>
			StateType* currStatePtr_ = nullptr;

			/// <summary>
			/// ステート変更リクエスト
			/// </summary>
			std::optional<StateEnum> stateRequest_;

			/// <summary>
			/// ステートロック数
			/// </summary>
			uint32_t lockCount_ = 0;

			/// <summary>
			/// ステート実体管理
			/// </summary>
			std::unordered_map<StateEnum, std::unique_ptr<StateType>> stateMap_;

			/// <summary>
			/// 登録済みステート一覧(ImGui用)
			/// </summary>
			std::vector<StateEnum> stateList_;

			/// <summary>
			/// ステート名管理(ImGui用)
			/// </summary>
			std::unordered_map<StateEnum, std::string> stateNames_;
		};
	}
}
