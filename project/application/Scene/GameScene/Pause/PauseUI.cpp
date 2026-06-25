#include "PauseUI.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "mathFunc.h"
#include "Timer.h"
#include "Ease.h"
#include "imgui.h"


using namespace TYEngine::Utility;
using namespace TYEngine::Framework;
using namespace TYEngine::Graphics;

void PauseUI::Init()
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


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/MasterVolume.png");
	volumeControl_[static_cast<int>(VolumeCategory::Master)].text = std::make_unique<Sprite>();
	volumeControl_[static_cast<int>(VolumeCategory::Master)].text->Initialize("Resources/Texture/MasterVolume.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/BGMVolume.png");
	volumeControl_[static_cast<int>(VolumeCategory::BGM)].text = std::make_unique<Sprite>();
	volumeControl_[static_cast<int>(VolumeCategory::BGM)].text->Initialize("Resources/Texture/BGMVolume.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/SEVolume.png");
	volumeControl_[static_cast<int>(VolumeCategory::SE)].text = std::make_unique<Sprite>();
	volumeControl_[static_cast<int>(VolumeCategory::SE)].text->Initialize("Resources/Texture/SEVolume.png");

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/UIVolume.png");
	volumeControl_[static_cast<int>(VolumeCategory::UI)].text = std::make_unique<Sprite>();
	volumeControl_[static_cast<int>(VolumeCategory::UI)].text->Initialize("Resources/Texture/UIVolume.png");




	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Slide.png");

	int index = 0;
	for (auto&& vol : volumeControl_)
	{
		vol.bar = std::make_unique<Sprite>();
		vol.bar->Initialize("Resources/Texture/white2x2.png");
		vol.bar->SetAnchorPoint(Vector2(0.5f, 0.5f));
		vol.bar->SetSize(Vector2(416.0f, 32.0f));
		vol.bar->SetPosition(Vector2(740.0f, 96.0f * float(index + 1)));
		vol.colorBar = std::make_unique<Sprite>();
		vol.colorBar->Initialize("Resources/Texture/white2x2.png");
		vol.colorBar->SetAnchorPoint(Vector2(0.5f, 0.5f));
		vol.colorBar->SetSize(Vector2(416.0f, 32.0f));
		vol.colorBar->SetPosition(Vector2(740.0f, 96.0f * float(index + 1)));

		


		vol.text->SetAnchorPoint(Vector2(0.5f, 0.5f));
		vol.text->SetPosition(Vector2(400.0f, 96.0f * float(index + 1)));



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
			if (vol.soundCategory == SoundCategory::CategoryNum)
			{
				vol.colorBar->SetColor(Vector4{ 1.0f,0.0f,0.0f,1.0f });
				volume = configJM_->Get<float>("Volume.Master");
			}
			else if (vol.soundCategory == SoundCategory::BGM)
			{
				vol.colorBar->SetColor(Vector4{ 1.0f,1.0f,0.0f,1.0f });
				volume = configJM_->Get<float>("Volume.BGM");
			}
			else if (vol.soundCategory == SoundCategory::SE)
			{
				vol.colorBar->SetColor(Vector4{ 0.0f,1.0f,0.0f,1.0f });
				volume = configJM_->Get<float>("Volume.SE");
			}
			else if (vol.soundCategory == SoundCategory::UI)
			{
				vol.colorBar->SetColor(Vector4{ 0.0f,0.0f,1.0f,1.0f });
				volume = configJM_->Get<float>("Volume.UI");
			}
			pos.x = Lerp(minX, maxX, volume);

			float len = pos.x - minX;
			vol.colorBar->SetSize(Vector2(len, 32.0f));
			vol.colorBar->SetPosition(Vector2(minX + (len / 2.0f), 96.0f * float(index + 1)));
		}


		vol.slide->SetPosition(pos);
		vol.size = vol.slide->GetSize();

		vol.pos = pos;

		++index;
	}

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/TitleReturn.png");
	menuButtons_[RETURN_TITLE].button = std::make_unique<Sprite>();
	menuButtons_[RETURN_TITLE].button->Initialize("Resources/Texture/TitleReturn.png");
	menuButtons_[RETURN_TITLE].button->SetPosition(Vector2(832.0f, 528.0f));
	menuButtons_[RETURN_TITLE].elements = RETURN_TITLE;


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Resume.png");
	menuButtons_[RESUME].button = std::make_unique<Sprite>();
	menuButtons_[RESUME].button->Initialize("Resources/Texture/Resume.png");
	menuButtons_[RESUME].button->SetPosition(Vector2(448.0f, 528.0f));
	menuButtons_[RESUME].elements = RESUME;

	for (auto&& button : menuButtons_)
	{
		button.button->SetAnchorPoint(Vector2(0.5f, 0.5f));
		button.size = button.button->GetSize();
		button.pos = button.button->GetPosition();
	}
}

void PauseUI::Reset()
{
	elements_ = ButtonElements::Num;
	isPush_ = false;
	timer_ = 0.0f;
	for (auto& vol : volumeControl_)
	{
		vol.isPush = false; 
		vol.hoverProgress = 0.0f;
	}
	for (auto&& button : menuButtons_)
	{
		button.hoverProgress = 0.0f;
	}
}

void PauseUI::Update()
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

	timer_ += Timer::GetInstance()->GetDeltaTime();

	back_->Update();
	text_->Update();

	// 音量調節バーの操作更新
	VolumeChange();
	// メニューボタン（再開・タイトルへ）操作更新
	ButtonProcess();

	for (auto& vol : volumeControl_)
	{
		vol.bar->Update();
		vol.colorBar->Update();
		vol.slide->Update();
		vol.text->Update();
	}
	for (auto& button : menuButtons_)
	{
		button.button->Update();
	}
}

void PauseUI::Draw()
{
	back_->Draw();
	text_->Draw();

	for (auto& vol : volumeControl_)
	{
		vol.bar->Draw();
		vol.colorBar->Draw();
		vol.slide->Draw();
		vol.text->Draw();
	}
	for (auto&& button : menuButtons_)
	{
		button.button->Draw();
	}
}

void PauseUI::DebugJMApply()
{
	back_->SetSize(jm_->Get<Vector2>("Pause.blackScreen.Size"));
	back_->SetTextureSize(jm_->Get<Vector2>("Pause.blackScreen.TextureSize"));
	back_->SetColor(jm_->Get<Vector4>("Pause.blackScreen.Color"));
	text_->SetPosition(jm_->Get<Vector2>("Pause.PauseText.Position"));
}

void PauseUI::ButtonProcess()
{
	if (isPush_) return; // スライドバー操作中はボタン反応しない

	Vector2 mousePos = input_->GetMousePosition();
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

	// アニメーションの速度（1.0 / 秒数） 
	// 例: 0.2秒で最大にしたい場合は 5.0f
	const float kAnimeSpeed = 5.0f;
	const float kMaxScale = 1.2f; // 最大1.2倍にスケーリング

	for (auto&& button : menuButtons_)
	{
		// マウスカーソルとの当たり判定
		bool isHovered = (std::abs(mousePos.x - button.pos.x) <= button.size.x / 2.0f) &&
			(std::abs(mousePos.y - button.pos.y) <= button.size.y / 2.0f);

		// ホバー演出: タイマー（進捗度）の更新
		if (isHovered)
		{
			button.hoverProgress += kAnimeSpeed * deltaTime;
			button.button->SetColor(Vector4{ 220.0f/255.0f, 220.0f/255.0f, 60.0f/255.0f, 1.0f });
		}
		else
		{
			button.hoverProgress -= kAnimeSpeed * deltaTime;
			button.button->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 1.0f });
		}
		// 0.0f ～ 1.0f の間にクランプ
		button.hoverProgress = std::clamp<float>(button.hoverProgress, 0.0f, 1.0f);

		// イージングを適用してサイズを計算（拡縮演出）
		float t = EaseFixed::OutBack(button.hoverProgress);

		// 線形補間でスケール値を決定 (1.0f から kMaxScale への補間)
		float currentScale = 1.0f + (kMaxScale - 1.0f) * t;
		button.button->SetSize(button.size * currentScale);

		// クリック処理：ステート遷移指示を設定
		if (isHovered && input_->IsTriggerMouse(0))
		{
			elements_ = button.elements;
			break;
		}
	}
}

void PauseUI::VolumeChange()
{
	float deltaTime = Timer::GetInstance()->GetDeltaTime();

	// アニメーションの速度（1.0 / 秒数） 
	// 例: 0.2秒で最大にしたい場合は 5.0f
	const float kAnimeSpeed = 5.0f;
	const float kMaxScale = 1.2f; // 最大1.2倍にスケーリング

	if (!isPush_)
	{
		Vector2 mousePos = input_->GetMousePosition();
		for (auto&& vol : volumeControl_)
		{
			vol.hoverProgress -= kAnimeSpeed * deltaTime;

			if ((std::abs(mousePos.x - vol.pos.x) <= vol.size.x / 2.0f) &&
				(std::abs(mousePos.y - vol.pos.y) <= vol.size.y / 2.0f))
			{
				vol.slide->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
				if (input_->IsTriggerMouse(0))
				{
					audio_->Play("slide", false, SoundCategory::UI);
					vol.isPush = true;
					isPush_ = true;
					break;
				}
			}
			else
			{
				vol.slide->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}
	}


	if (input_->IsPressMouse(0) && isPush_)
	{
		int index = 0;
		for (auto&& vol : volumeControl_)
		{
			vol.slide->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			if (vol.isPush)
			{
				vol.hoverProgress += kAnimeSpeed * deltaTime;

				if (vol.bar->GetSize().x != 0.0f)
				{
					float minX = vol.bar->GetPosition().x - (vol.bar->GetSize().x / 2.0f);
					float maxX = vol.bar->GetPosition().x + (vol.bar->GetSize().x / 2.0f);
					vol.pos.x = std::clamp(input_->GetMousePosition().x, minX, maxX);
					float liner = maxX - minX;
					float t = (vol.pos.x - minX) / liner;

					if (vol.soundCategory != SoundCategory::CategoryNum)
					{
						audio_->SetCategoryVolume(vol.soundCategory, t);
					}
					else
					{
						audio_->SetMasterVolume(t);
					}

					if (vol.soundCategory == SoundCategory::CategoryNum)
					{
						configJM_->Set("Volume.Master", t);
					}
					else if (vol.soundCategory == SoundCategory::BGM)
					{
						configJM_->Set("Volume.BGM", t);
					}
					else if (vol.soundCategory == SoundCategory::SE)
					{
						configJM_->Set("Volume.SE", t);
					}
					else if (vol.soundCategory == SoundCategory::UI)
					{
						configJM_->Set("Volume.UI", t);
					}

					float len = vol.pos.x - minX;
					vol.colorBar->SetSize(Vector2(len, 32.0f));
					vol.colorBar->SetPosition(Vector2(minX + (len / 2.0f), 96.0f * float(index + 1)));
				}

				vol.slide->SetColor(Vector4(0.9f, 0.7f, 0.2f, 1.0f));
				vol.slide->SetPosition(vol.pos);
				configJM_->Save();
			}
			++index;
		}
	}
	else if (isPush_)
	{
		for (auto&& vol : volumeControl_)
		{
			vol.hoverProgress -= kAnimeSpeed * deltaTime;
			vol.slide->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			vol.isPush = false;
		}
		isPush_ = false;
	}

	for (auto&& vol : volumeControl_)
	{
		// 0.0f ～ 1.0f の間にクランプ
		vol.hoverProgress = std::clamp<float>(vol.hoverProgress, 0.0f, 1.0f);

		// イージングを適用してサイズを計算
		float t = EaseFixed::OutBack(vol.hoverProgress);

		// 線形補間でスケール値を決定 (1.0f から kMaxScale への補間)
		float currentScale = 1.0f + (kMaxScale - 1.0f) * t;
		vol.slide->SetSize(vol.size * currentScale);
	}
}
