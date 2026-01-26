#include "ModelManager.h"

void ModelManager::Initialize(DirectXBasis* dxManager, SrvManager* srvManager)
{
	modelLoader_ = std::make_unique<ModelLoader>();
	modelLoader_->Initialize(dxManager, srvManager);
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& fileName)
{
	if (models_.contains(fileName)) return;// 読み込み済みなら早期 return

	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelLoader_.get(), directoryPath, fileName);

	models_.insert(std::make_pair(fileName, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	if (models_.contains(filePath)) return models_.at(filePath).get();// 読み込みモデルを戻り値として return

	// ファイル名が一致しなければ null
	return nullptr;
}