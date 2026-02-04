#include "PlayUI.h"
#include "Input.h"
#include "mathFunc.h"
#include "TextureManager.h"
#include "../../../ScoreUI/ScoreUI.h"

void PlayUI::Init()
{
	input_ = Input::GetInstance();

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/reticle.png");
	reticle_ = std::make_unique<Sprite>();
	reticle_->Initialize("Resources/Texture/reticle.png");
	reticle_->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.reticle.AnchorPoint"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ComboText.png");
	sprites_[COMBO_TEXT] = std::make_unique<Sprite>();
	sprites_[COMBO_TEXT]->Initialize("Resources/Texture/ComboText.png");
	sprites_[COMBO_TEXT]->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.ComboText.AnchorPoint"));
	sprites_[COMBO_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.ComboText.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");
	sprites_[COMBO_NUM_TEXT] = std::make_unique<Sprite>();
	sprites_[COMBO_NUM_TEXT]->Initialize("Resources/Texture/number.png");
	sprites_[COMBO_NUM_TEXT]->SetTextureSize(jm_->Get<Vector2>("PlayUI.Texture.number.TextureSize"));
	sprites_[COMBO_NUM_TEXT]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.number.Size"));
	sprites_[COMBO_NUM_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.number.PositionCombo"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/HpText.png");
	sprites_[HP_TEXT] = std::make_unique<Sprite>();
	sprites_[HP_TEXT]->Initialize("Resources/Texture/HpText.png");
	sprites_[HP_TEXT]->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.HpText.AnchorPoint"));
	sprites_[HP_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.HpText.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/number.png");
	sprites_[HP_NUM_TEXT] = std::make_unique<Sprite>();
	sprites_[HP_NUM_TEXT]->Initialize("Resources/Texture/number.png");
	sprites_[HP_NUM_TEXT]->SetTextureSize(jm_->Get<Vector2>("PlayUI.Texture.number.TextureSize"));
	sprites_[HP_NUM_TEXT]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.number.Size"));
	sprites_[HP_NUM_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.number.PositionHp"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Operation.png");
	sprites_[OPERATION] = std::make_unique<Sprite>();
	sprites_[OPERATION]->Initialize("Resources/Texture/Operation.png");
	sprites_[OPERATION]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.Operation.Size"));
	sprites_[OPERATION]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.Operation.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/outline.png");
	sprites_[OUTLINE] = std::make_unique<Sprite>();
	sprites_[OUTLINE]->Initialize("Resources/Texture/outline.png");
	sprites_[OUTLINE]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.Operation.Size"));
	sprites_[OUTLINE]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.Operation.Position"));

	TextureManager::GetInstance()->LoadTexture("Resources/Texture/ReturnTitle.png");
	sprites_[PAUSE] = std::make_unique<Sprite>();
	sprites_[PAUSE]->Initialize("Resources/Texture/ReturnTitle.png");
	sprites_[PAUSE]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.ReturnTitle.Size"));


	TextureManager::GetInstance()->LoadTexture("Resources/Texture/Shift.png");
	sprites_[SHIFT] = std::make_unique<Sprite>();
	sprites_[SHIFT]->Initialize("Resources/Texture/Shift.png");
	sprites_[SHIFT]->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.HpText.AnchorPoint"));
}

void PlayUI::DebugJMApply()
{
	reticle_->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.reticle.AnchorPoint"));

	sprites_[COMBO_TEXT]->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.ComboText.AnchorPoint"));

	sprites_[COMBO_NUM_TEXT]->SetTextureSize(jm_->Get<Vector2>("PlayUI.Texture.number.TextureSize"));
	sprites_[COMBO_NUM_TEXT]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.number.Size"));

	sprites_[HP_TEXT]->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.HpText.AnchorPoint"));
	sprites_[HP_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.HpText.Position"));

	sprites_[HP_NUM_TEXT]->SetTextureSize(jm_->Get<Vector2>("PlayUI.Texture.number.TextureSize"));
	sprites_[HP_NUM_TEXT]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.number.Size"));
	sprites_[HP_NUM_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.number.PositionHp"));

	sprites_[OPERATION]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.Operation.Size"));
	sprites_[OPERATION]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.Operation.Position"));

	sprites_[OUTLINE]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.Operation.Size"));
	sprites_[OUTLINE]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.Operation.Position"));

	sprites_[PAUSE]->SetSize(jm_->Get<Vector2>("PlayUI.Texture.ReturnTitle.Size"));

	sprites_[SHIFT]->SetAnchorPoint(jm_->Get<Vector2>("PlayUI.Texture.HpText.AnchorPoint"));
}

void PlayUI::SetShiftPos(const Vector2& pos)
{
	float windowWidth = WindowsApp::kClientWidth;
	float windowHeight = WindowsApp::kClientHeight;

	Vector2 pixelPos;
	pixelPos.x = (pos.x + 1.0f) * 0.5f * windowWidth;

	pixelPos.y = ((1.0f - pos.y) * 0.5f * windowHeight) + 30.0f;
	sprites_[SHIFT]->SetPosition(pixelPos);
}

void PlayUI::Update()
{
#ifdef _DEBUG
	DebugJMApply();
#endif // _DEBUG

	for (auto& sprite : sprites_)
	{
		sprite->Update();
	}
	scoreDraw_->Update();
	ComboTexUpdate();
}

void PlayUI::Draw()
{
	reticle_->Draw();

	for (int i = 0; i < SpriteNum; ++i)
	{
		if (i != OUTLINE && i != SHIFT)
		{
			sprites_[i]->Draw();
		}
		else
		{
			if (isJust_)
			{
				// アウトライン描画
				sprites_[i]->Draw();
			}
		}
	}
	scoreDraw_->Draw();
}

void PlayUI::DrawRT()
{
	reticle_->Update();
}

void PlayUI::ComboTexUpdate()
{
	Vector2 mouse = input_->GetMousePosition();
	reticle_->SetPosition(mouse);

	// コンボ表示の透明度制御
	float t = comboTimer_ / kComboTime_;
	t = 1.0f - powf(1.0f - t, 4.0f);
	sprites_[COMBO_TEXT]->SetAlpha(t);
	sprites_[COMBO_NUM_TEXT]->SetAlpha(t);

	// コンボ終了間際のシェイク演出
	t = (comboTimer_ - (kComboTime_ - shakeTime_)) / (kComboTime_ - (kComboTime_ - shakeTime_));
	if (t > 0)
	{
		std::mt19937 random(seedGene_());
		std::uniform_real_distribution<float> dist(-15.0f, 15.0f);
		Vector2 pos = { dist(random),dist(random) };
		sprites_[COMBO_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.ComboText.Position") + pos * t);
		sprites_[COMBO_NUM_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.number.PositionCombo") + pos * t);
	}
	else
	{
		sprites_[COMBO_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.ComboText.Position"));
		sprites_[COMBO_NUM_TEXT]->SetPosition(jm_->Get<Vector2>("PlayUI.Texture.number.PositionCombo"));
	}
}
