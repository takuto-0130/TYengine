#pragma once
#include <memory>

#include "Utils/Json/JsonManager.h"
#include "Input.h"

#include "../../../AppSystem/Audio/GameAudio.h"

class Sprite;

enum ButtonElements
{
	RESUME,
	RETURN_TITLE,
	Num
};

class PauseClass
{
public:
	void Init();

	void Reset();

	void Update();

	void Draw();

	void SetJsonManager(jx::JsonManager* jm) { jm_ = jm; }

	void SetConfigJsonManager(jx::JsonManager* jm) { configJM_ = jm; }

	ButtonElements GetElements() { return elements_; }

private:
	void DebugJMApply();

	void ButtonProcess();

	void VolumeChange();

private:
	struct VolumeControl
	{
		SoundCategory soundCategory = SoundCategory::CategoryNum;
		Vector2 pos = {};	// 判定用座標
		Vector2 size = {};	// 判定用サイズ
		bool isPush = false;
		float hoverProgress = 0.0f;
		std::unique_ptr<Sprite> bar;
		std::unique_ptr<Sprite> colorBar;
		std::unique_ptr<Sprite> slide;
		std::unique_ptr<Sprite> text;
	};

	enum VolumeCategory
	{
		Master,
		BGM,
		SE,
		UI,
		CategoryNum
	};

	std::array<VolumeControl, VolumeCategory::CategoryNum> volumeControl_;

	struct MenuButton
	{
		ButtonElements elements = ButtonElements::Num;
		Vector2 pos = {};	// 判定用座標
		Vector2 size = {};	// 判定用サイズ
		std::unique_ptr<Sprite> button;
		float hoverProgress = 0.0f;
	};

	std::array<MenuButton, ButtonElements::Num> menuButtons_;

	bool isPush_ = false;
	ButtonElements elements_ = ButtonElements::Num;

	Input* input_ = nullptr;
	GameAudio* audio_ = nullptr;

	float timer_ = 0.0f;


	std::unique_ptr<Sprite> returnTitle_;
	std::unique_ptr<Sprite> resume_;

	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;

	jx::JsonManager* jm_;
	jx::JsonManager* configJM_;
};

