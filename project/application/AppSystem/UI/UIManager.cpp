#include "UIManager.h"

BaseUI* UIManager::GetUI(const std::string& name)
{
	auto it = uiMap_.find(name);
	if (it != uiMap_.end()) return it->second.get();
	return nullptr;
}

void UIManager::SetJsonManager(TYEngine::Utility::JsonManager* jm)
{
	jm_ = jm;
	for (auto& pair : uiMap_) {
		pair.second->SetJsonManager(jm_);
	}
}

void UIManager::InitAll()
{
	for (auto& pair : uiMap_) {
		pair.second->Init();
	}
}

void UIManager::UpdateUI(const std::string& name)
{
	auto* ui = GetUI(name);
	if (ui) {
		ui->Update();
	}
}

void UIManager::DrawUI(const std::string& name)
{
	auto* ui = GetUI(name);
	if (ui) {
		ui->Draw();
	}
}

void UIManager::Clear()
{
	uiMap_.clear();
}
