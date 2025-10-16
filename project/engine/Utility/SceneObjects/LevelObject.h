#pragma once
#include "BaseObject.h"

// blenderで作成したレベルエディタの配置オブジェクトを受け取るクラス
class LevelObject :
    public BaseObject
{
public:
    void Init() override;

    void Update()override;

    void Draw()override;

public:
    void SetModelName(std::string modelName) { modelName_ = modelName; }
    void SetPosition(Vector3 pos) { worldTransform_.translation_ = pos; }
    void SetRotation(Vector3 rota) { worldTransform_.rotation_ = rota; }
    void SetScale(Vector3 scale) { worldTransform_.colliderScale_ = scale; }

private:
    std::string modelName_;
};

