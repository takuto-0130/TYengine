#pragma once
#include "IScene.h"
#include "LevelObject.h"
#include "BlenderLevelLoader.h"
#include "ObjectCubemap.h"

#include "Sprite.h"
class TitleScene : public IScene {
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;

private:
	void LoadLevel();

	void Transition();

private:
	std::vector<std::unique_ptr<LevelObject>> objects_;
	std::unique_ptr<BlenderLevelLoader> loader_;

	std::unique_ptr<Sprite> spaceSpr_;
	std::unique_ptr<Sprite> text_;


	std::unique_ptr<ObjectCubemap> skybox_;
};
