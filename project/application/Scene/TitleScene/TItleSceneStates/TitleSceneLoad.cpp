#include "../TitleScene.h"

#include "TextureManager.h"

#include "../../../AppSystem/Audio/GameAudio.h"

using namespace TYEngine::Graphics;
using namespace TYEngine::Debugger;

void TitleScene::Load()
{
	//========== ロード ==========//
	
	// タイトル用JSONデータのロード
	titleJM.Load("Title.json", true, &err);
#ifdef _DEBUG
	Log(err);
#endif // _DEBUG
	gameAudio_ = GameAudio::GetInstance();

	//========== サウンドリソースロード ==========//
	gameAudio_->LoadSound("open");
	gameAudio_->LoadSound("close");
	gameAudio_->LoadSound("slide");
	gameAudio_->LoadSound("enter");
	gameAudio_->LoadSound("fanfare");
	gameAudio_->LoadSound("gameBGM");
	gameAudio_->LoadSound("gekiha");
	gameAudio_->LoadSound("damageE");
	gameAudio_->LoadSound("damageP");
	gameAudio_->LoadSound("roll");
	gameAudio_->LoadSound("attack");
	gameAudio_->LoadSound("418");
	gameAudio_->LoadSound("irodori", ".mp3");
	
	gameAudio_->InitBeatAnalyzer("gameBGM", SoundCategory::BGM);

	//========== テクスチャロード ==========//

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Enter.png");
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Title.png");
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Operation.png");
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/white2x2.png");
}