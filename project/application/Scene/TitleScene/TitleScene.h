#pragma once
#include "IScene.h"
#include "ObjectCubemap.h"
#include "Object/Player/Player.h"
#include "../../Object/Enemy/TitleEnemy/TitleEnemyManager.h"

#include "Audio/AudioAnalyzer.h"

#include "Audio/AudioAnalyzer.h"

#define JSONMGR_WITH_IMGUI
#include "Utils/Json/JsonManager.h"

#include "Sprite.h"
class TitleScene : public IScene {
public:
	~TitleScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;

private:
	void Transition();

	void DebugJMApply();

private:
	std::unique_ptr<Sprite> spaceSpr_;
	std::unique_ptr<Sprite> text_;
	std::unique_ptr<Sprite> operation_;
	std::unique_ptr<Sprite> reticle_;

	std::unique_ptr<ObjectCubemap> skybox_;

	std::unique_ptr<Player> player_;

	TitleEnemyManager enemyMgr_;


	std::unique_ptr<Object3d> ground_;
	WorldTransform groundWT_;
	float rotateSpeed_ = 0.0f;

	jx::JsonManager titleJM;
	std::string err;

	AudioAnalyzer audioAnalyzer_;
};
