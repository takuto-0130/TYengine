#pragma once
#include "BaseObject.h"

class Skydome
	: public BaseObject
{
public:

	/// 初期化
	void Init() override;

	/// 更新
	void Update()override;

	/// 描画
	void Draw()override;
};

