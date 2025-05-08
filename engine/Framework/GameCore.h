#pragma once
#include "TYFrameWork.h"
#include "IScene.h"
#include "ParticleManager.h"

class GameCore : public TYFrameWork
{
public: // メンバ変数

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

private:
	Input* input = nullptr;

	std::unique_ptr<ParticleManager> particleManager;
};

