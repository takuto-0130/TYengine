#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"
#include "Input.h"

#include "../../../AppSystem/Audio/GameAudio.h"

class Sprite;

class PauseClass
{
public:
	void Init();

	void Update();

	void Draw();

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

	void SetConfigJsonManager(jx::JsonManager* jm) { configJM_ = jm; }

private:
	void DebugJMApply();

	void VolumeChange();

private:
	struct VolumeControl
	{
		SoundCategory soundCategory = SoundCategory::CategoryNum;
		Vector2 pos = {};	// 判定用座標
		Vector2 size = {};	// 判定用サイズ
		bool isPush = false;
		std::unique_ptr<Sprite> bar;
		std::unique_ptr<Sprite> colorBar;
		std::unique_ptr<Sprite> slide;
	};

	enum VolumeCategory
	{
		Master,
		BGM,
		SE,
		UI,
		CategoryNum
	};

	std::array<VolumeControl, static_cast<int>(VolumeCategory::CategoryNum)> volumeControl_;

	bool isPush = false;
	Input* input_ = nullptr;
	GameAudio* audio_ = nullptr;


	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;

	jx::JsonManager* jm_;
	jx::JsonManager* configJM_;
};

