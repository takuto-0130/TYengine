#include "Object3d.h"
#include "Object3dBasis.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include <numbers>


namespace TYEngine
{
	namespace Graphics
	{

		using namespace Utility;

		void Object3d::Initialize()
		{
			objectManager_ = Object3dBasis::GetInstance();

			// マテリアル、カメラ用定数バッファ生成
			CreateMaterialResource();
			CreateCameraResource();
		}

		void Object3d::Draw(WorldTransform& worldTransform)
		{
			camera_ = objectManager_->GetDefaultCamera();
			cameraData_->worldPosition = camera_->GetTranslate();

			// cameraの場所を指定
			objectManager_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());
			// マテリアルCBufferの場所を指定
			objectManager_->GetDirectXBasis()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
			// 3Dモデルが割り当てられていれば描画する
			if (model_)
			{
				model_->Draw(worldTransform, camera_);
			}
		}

		void Object3d::CreateMaterialResource()
		{
			// マテリアル用のリソースを作る。今回はMaterial1つ分のサイズを用意する
			materialResource_ = objectManager_->GetDirectXBasis()->CreateBufferResource(sizeof(Material));
			// 書き込むためのアドレスを取得
			materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
			materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白を入れる
			materialData_->addColor = { 0.0f, 0.0f, 0.0f, 0.0f }; // 黒を入れる
			materialData_->enableLighting = 1;
			materialData_->uvTransform = MakeIdentity4x4();
			materialData_->environmentCoefficient = 0.0f; // 映りこみなし
			materialData_->shininess = 10.0f;
		}

		void Object3d::CreateCameraResource()
		{
			// カメラ用のリソースを作る
			cameraResource_ = objectManager_->GetDirectXBasis()->CreateBufferResource(sizeof(CameraForGPU));
			// 書き込むためのアドレスを取得
			cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
			// 初期値を入れる
			cameraData_->worldPosition = { 1.0f, 1.0f, 1.0f };
		}

		void Object3d::SetModel(const std::string& filePath)
		{
			// モデルを検索してセットする（読み込み済みかのチェック）
			model_ = ModelManager::GetInstance()->FindModel(filePath);

			// 該当モデルがロードされていなければロードしてセット
			if (model_ == nullptr)
			{
				// 拡張子を取り除く処理
				std::string fileName = filePath;
				std::string basePath;
				if (fileName.size() > 4)
				{
					// .obj または .gltf の場合に削除
					if (fileName.substr(fileName.size() - 4) == ".obj")
					{
						basePath = fileName.substr(0, fileName.size() - 4);
					}
					else if (fileName.size() > 5 && fileName.substr(fileName.size() - 5) == ".gltf")
					{
						basePath = fileName.substr(0, fileName.size() - 5);
					}
				}

				// ロードしてセット
				ModelManager::GetInstance()->LoadModel(defaultModelsPath_ + basePath, fileName);
				model_ = ModelManager::GetInstance()->FindModel(filePath);
			}

		}

	} // namespace Graphics
} // namespace TYEngine
