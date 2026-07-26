#pragma once
#include "Utils/Json/JsonManager.h"

/// <summary>
/// 全てのUIエレメントの抽象基底クラス。
/// 画面UIの初期化・更新・描画およびJSON設定マネージャの管理インターフェースを規定する。
/// </summary>
class BaseUI
{
public:
	virtual ~BaseUI() = default;
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void SetJsonManager(TYEngine::Utility::JsonManager* jm) { jm_ = jm; }

protected:
	TYEngine::Utility::JsonManager* jm_ = nullptr;
};
