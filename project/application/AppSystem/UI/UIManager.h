#pragma once
#include "SingletonObject.h"
#include "BaseUI.h"
#include <unordered_map>
#include <string>
#include <memory>

/// <summary>
/// UIエレメント管理マネージャクラス。
/// アプリケーション全体のUIの登録・取得・一括更新・一括描画を管理する。
/// </summary>
class UIManager : public TYEngine::Utility::SingletonObject<UIManager>
{
	friend class TYEngine::Utility::SingletonObject<UIManager>;
	friend struct std::default_delete<UIManager>;

private:
	UIManager() = default;
	~UIManager() override = default;

public:
	/// <summary>UIの登録</summary>
	template <typename T, typename... Args>
	T* RegisterUI(const std::string& name, Args&&... args)
	{
		auto ui = std::make_unique<T>(std::forward<Args>(args)...);
		if (jm_) {
			ui->SetJsonManager(jm_);
		}
		T* ptr = ui.get();
		uiMap_[name] = std::move(ui);
		return ptr;
	}

	/// <summary>UIの取得</summary>
	BaseUI* GetUI(const std::string& name);

	template <typename T>
	T* GetUI(const std::string& name)
	{
		return dynamic_cast<T*>(GetUI(name));
	}

	/// <summary>JsonManagerを設定し、登録済みの全UIに伝搬する</summary>
	void SetJsonManager(TYEngine::Utility::JsonManager* jm);

	/// <summary>登録された全てのUIを一括初期化する</summary>
	void InitAll();

	/// <summary>指定したUIの更新処理</summary>
	void UpdateUI(const std::string& name);

	/// <summary>指定したUIの描画処理</summary>
	void DrawUI(const std::string& name);

	/// <summary>全ての登録済みUIをクリアする（シーン遷移時など）</summary>
	void Clear();

private:
	std::unordered_map<std::string, std::unique_ptr<BaseUI>> uiMap_;
	TYEngine::Utility::JsonManager* jm_ = nullptr;
};
