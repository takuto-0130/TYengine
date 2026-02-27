#include "../TitleScene.h"
#include "../../../AppSystem/Audio/GameAudio.h"
#include "Timer.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG

using namespace TYEngine::Utility;

void TitleScene::DebugUpdate()
{
#ifdef _DEBUG
	audioAnalyzer_.Draw();
	beatAnalyzer_.Draw();
	/*static int bgmH = -1;

	if (input_->TriggerKey(DIK_M))
	{
		bgmH = gameAudio_->Play("418", false, SoundCategory::BGM);
		gameAudio_->SetSoundVolume(bgmH, 1.0f);
	}
	if (input_->TriggerKey(DIK_P))
	{
		gameAudio_->Pause(bgmH);
	}
	if (input_->TriggerKey(DIK_O))
	{
		gameAudio_->Resume(bgmH);
	}*/

	// ImGui で編集
	ImGui::Begin("JSON Editor");
	static JsonImGuiEditor inspector(titleJM);
	inspector.Draw(titleJM.Root(), "Title.json");
	if (ImGui::Button("Save")) titleJM.Save();
	ImGui::End();

	DebugJMApply();
#endif // _DEBUG
}

void TitleScene::DebugJMApply()
{
	enterSpr_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.Enter.AnchorPoint"));
	enterSpr_->SetPosition(titleJM.Get<Vector2>("config.texture.Enter.Position"));
	text_->SetPosition(titleJM.Get<Vector2>("config.texture.Title.Position"));
	camera_->SetRotate(titleJM.Get<Vector3>("config.Camera.Rotate"));
	camera_->SetTranslate(titleJM.Get<Vector3>("config.Camera.Position"));
	operation_->SetPosition(titleJM.Get<Vector2>("config.texture.Operation.Position"));
	reticle_->SetAnchorPoint(titleJM.Get<Vector2>("config.texture.reticle.AnchorPoint"));
	rotateSpeed_ = titleJM.Get<float>("config.ground.rotateSpeed");
	oscillator_.SetDuration(titleJM.Get<float>("config.oscillator.floatingDuration"));
}