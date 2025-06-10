#pragma once
#include "IScene.h"
#include "LevelObject.h"
#include "BlenderLevelLoader.h"
class TitleScene : public IScene {
public:
	void Init() override;
	void Update() override;
	void Draw() override;

private:
	void LoadLevel();

private:
	std::vector<std::unique_ptr<LevelObject>> objects_;
	std::unique_ptr<BlenderLevelLoader> loader_;
};
