#pragma once
#include "struct.h"
#include <string>
#include <json.hpp>

class LevelObject;

struct LevelData
{
	struct ObjectData
	{
		std::string fileName = "";
		Vector3 translation = {};
		Vector3 rotation = {};
		Vector3 scaling = {};
		Vector3 colliderCenter = {};
		Vector3 colliderSize = {};
	};

	std::vector<ObjectData> objects;
};

// blenderで作成したレベルエディタの配置データを読み込むクラス
class BlenderLevelLoader
{
public:
	// コンストラクタ
	BlenderLevelLoader(const std::string& directoryPath) : kBaseDirectoryName_(directoryPath) {}

	// ロード
	LevelData* Load(const std::string& filename);

	// ロードしたデータを配置
	void DataToObject(LevelData* data, std::vector<std::unique_ptr<LevelObject>>& objects);

private:
	// オブジェクト走査
	void ObjectTraversal(LevelData* levelData, nlohmann::json& j, std::string contains);

private:
	std::string kBaseDirectoryName_;

	std::unique_ptr<LevelData> levelData_;
};

