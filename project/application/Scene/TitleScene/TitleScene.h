#pragma once
#include "IScene.h"
#include "LevelObject.h"
#include "BlenderLevelLoader.h"
#include "ObjectCubemap.h"
#include "Object/Player/Player.h"
#include "../../Object/Enemy/TitleEnemy/TitleEnemyManager.h"
#include "../Transition/Fade2/BlockFadeOverlay.h"

#include "Sprite.h"
class TitleScene : public IScene {
public:
	~TitleScene();

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

	std::unique_ptr<Player> player_;

	TitleEnemyManager enemyMgr_;


	std::unique_ptr<Object3d> ground_;
	WorldTransform groundWT_;

	std::unique_ptr<Sprite> operation_;

	std::unique_ptr<Sprite> reticle_;

	//BlockFadeOverlay fadeOverlay_;
};
