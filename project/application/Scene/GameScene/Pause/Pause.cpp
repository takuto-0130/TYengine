#include "Pause.h"
#include "Sprite.h"
#include "TextureManager.h"

void PauseClass::Init()
{
	TextureManager::GetInstance()->LoadTexture("Resources/Texture/blackScreen.png");
	back_ = std::make_unique<Sprite>();
	back_->Initialize("Resources/Texture/blackScreen.png");
	back_->SetSize(jm_->Get<Vector2>("Pause.blackScreen.Size"));
	back_->SetTextureSize(jm_->Get<Vector2>("Pause.blackScreen.TextureSize"));
	back_->SetColor(jm_->Get<Vector4>("Pause.blackScreen.Color"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/PauseText.png");
	text_ = std::make_unique<Sprite>();
	text_->Initialize("Resources/Texture/PauseText.png");
	text_->SetPosition(jm_->Get<Vector2>("Pause.PauseText.Position"));
}

void PauseClass::Update()
{
#ifdef _DEBUG
	DebugJMApply();
#endif // _DEBUG

	back_->Update();
	text_->Update();
}

void PauseClass::Draw()
{
	back_->Draw();
	text_->Draw();
}

void PauseClass::DebugJMApply()
{
	back_->SetSize(jm_->Get<Vector2>("Pause.blackScreen.Size"));
	back_->SetTextureSize(jm_->Get<Vector2>("Pause.blackScreen.TextureSize"));
	back_->SetColor(jm_->Get<Vector4>("Pause.blackScreen.Color"));
	text_->SetPosition(jm_->Get<Vector2>("Pause.PauseText.Position"));
}
