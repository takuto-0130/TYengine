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

class BlenderLevelLoader
{
public:
	BlenderLevelLoader(std::string directoryPath) : kBaseDirectoryName_(directoryPath) {}

	LevelData* Load(const std::string& filename);

	void DataToObject(LevelData* data, std::vector<std::unique_ptr<LevelObject>>& objects);

private:
	void ObjectTraversal(LevelData* levelData, nlohmann::json& j, std::string contains);

private:
	std::string kBaseDirectoryName_;
};

