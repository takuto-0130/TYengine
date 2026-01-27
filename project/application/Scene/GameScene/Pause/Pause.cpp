#include "Pause.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "mathFunc.h"
#include "imgui.h"

void PauseClass::Init()
{
	input_ = Input::GetInstance();
	audio_ = GameAudio::GetInstance();

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

	// 代入する SoundCategory で CategoryNum を Master として代用
	volumeControl_[static_cast<int>(VolumeCategory::Master)].soundCategory = SoundCategory::CategoryNum;
	volumeControl_[static_cast<int>(VolumeCategory::BGM)].soundCategory = SoundCategory::BGM;
	volumeControl_[static_cast<int>(VolumeCategory::SE)].soundCategory = SoundCategory::SE;
	volumeControl_[static_cast<int>(VolumeCategory::UI)].soundCategory = SoundCategory::UI;

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Slide.png");

	int index = 0;
	for (auto&& vol : volumeControl_)
	{
		vol.bar = std::make_unique<Sprite>();
		vol.bar->Initialize("Resources/Texture/white2x2.png");
		vol.bar->SetAnchorPoint(Vector2(0.5f, 0.5f));
		vol.bar->SetSize(Vector2(416.0f, 32.0f));
		vol.bar->SetPosition(Vector2(640.0f, 96.0f * float(index + 1)));
		vol.colorBar = std::make_unique<Sprite>();
		vol.colorBar->Initialize("Resources/Texture/white2x2.png");
		vol.colorBar->SetAnchorPoint(Vector2(0.5f, 0.5f));
		vol.colorBar->SetSize(Vector2(416.0f, 32.0f));
		vol.colorBar->SetPosition(Vector2(640.0f, 96.0f * float(index + 1)));

		vol.slide = std::make_unique<Sprite>();
		vol.slide->Initialize("Resources/Texture/Slide.png");
		vol.slide->SetAnchorPoint(Vector2(0.5f, 0.5f));

		Vector2 pos = {};
		pos.y = vol.bar->GetPosition().y;

		if (vol.bar->GetSize().x != 0.0f)
		{
			float minX = vol.bar->GetPosition().x - (vol.bar->GetSize().x / 2.0f);
			float maxX = vol.bar->GetPosition().x + (vol.bar->GetSize().x / 2.0f);
			float volume = 1.0f;
			pos.x = Lerp(minX, maxX, volume);
		}

		vol.slide->SetPosition(pos);
		vol.size = vol.slide->GetSize();

		vol.pos = pos;

		++index;
	}
}

void PauseClass::Update()
{
#ifdef _DEBUG
	DebugJMApply();

	ImGui::Begin("Volume");
	Vector2 mousePos = input_->GetMousePosition();
	ImGui::DragFloat2("mouse", &mousePos.x);
	for (auto& vol : volumeControl_)
	{
		Vector2 pos = vol.slide->GetPosition();
		ImGui::DragFloat2("spr", &pos.x);
	}
	float masterVol = audio_->GetMasterVolume();
	ImGui::DragFloat("master", &masterVol);
	for (int i = 1; i < static_cast<int>(VolumeCategory::CategoryNum); ++i)
	{
		float volume = audio_->GetCategoryVolume(volumeControl_[i].soundCategory);
		ImGui::DragFloat("cate", &volume);
	}
	ImGui::End();

#endif // _DEBUG

	back_->Update();
	text_->Update();

	VolumeChange();

	for (auto& vol : volumeControl_)
	{
		vol.bar->Update();
		vol.colorBar->Update();
		vol.slide->Update();
	}
}

void PauseClass::Draw()
{
	back_->Draw();
	text_->Draw();

	for (auto& vol : volumeControl_)
	{
		vol.bar->Draw();
		vol.colorBar->Draw();
		vol.slide->Draw();
	}
}

void PauseClass::DebugJMApply()
{
	back_->SetSize(jm_->Get<Vector2>("Pause.blackScreen.Size"));
	back_->SetTextureSize(jm_->Get<Vector2>("Pause.blackScreen.TextureSize"));
	back_->SetColor(jm_->Get<Vector4>("Pause.blackScreen.Color"));
	text_->SetPosition(jm_->Get<Vector2>("Pause.PauseText.Position"));
}

void PauseClass::VolumeChange()
{
	if (input_->IsTriggerMouse(0))
	{
		Vector2 mousePos = input_->GetMousePosition();
		if (!isPush)
		{
			for (auto&& vol : volumeControl_)
			{
				if ((std::abs(mousePos.x - vol.pos.x) <= vol.size.x / 2.0f) &&
					(std::abs(mousePos.y - vol.pos.y) <= vol.size.y / 2.0f))
				{
					vol.isPush = true;
					isPush = true;
					break;
				}
			}
		}
	}

	if (input_->IsPressMouse(0) && isPush)
	{
		for (auto&& vol : volumeControl_)
		{
			vol.slide->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			if (vol.isPush)
			{
				if (vol.bar->GetSize().x != 0.0f)
				{
					float minX = vol.bar->GetPosition().x - (vol.bar->GetSize().x / 2.0f);
					float maxX = vol.bar->GetPosition().x + (vol.bar->GetSize().x / 2.0f);
					vol.pos.x = std::clamp(input_->GetMousePosition().x, minX, maxX);
					float liner = maxX - minX;
					float t = (vol.pos.x - minX) / liner;
				}

				if(vol.soundCategory != SoundCategory::CategoryNum) 
				{
					audio_->SetCategoryVolume(vol.soundCategory, t);
				}
				else
				{
					audio_->SetMasterVolume(t);
				}
				vol.slide->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
				vol.slide->SetPosition(vol.pos);
				// jsonManager_->Save();
			}
		}
	}
	else if (isPush)
	{
		for (auto&& vol : volumeControl_)
		{
			vol.slide->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			vol.isPush = false;
		}
		isPush = false;
	}
}
