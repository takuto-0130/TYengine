#pragma once
#include "BaseObject.h"

namespace TYEngine
{
	namespace Utility
	{
		/// <summary>
		/// Blender等のDCCツールで配置されたレベルオブジェクト。
		/// モデル名やトランスフォーム情報を保持し、描画を行う。
		/// </summary>
		class LevelObject :
			public BaseObject
		{
		public:
			/// <summary>初期化処理。</summary>
			void Init() override;

			/// <summary>更新処理。</summary>
			void Update()override;

			/// <summary>描画処理。</summary>
			void Draw()override;

		public:
			void SetModelName(const std::string& modelName) { modelName_ = modelName; }
			void SetPosition(const Vector3& pos) { worldTransform_.SetTranslation(pos); }
			void SetRotation(const Vector3& rotate) { worldTransform_.SetRotate(rotate); }
			void SetScale(const Vector3& scale) { worldTransform_.SetScale(scale); }
			void SetScale(float scale) { worldTransform_.SetScale(scale); }

			const std::string& GetModelName() { return modelName_; }

		private:
			std::string modelName_;
		};

	} // namespace Utility
} // namespace TYEngine

