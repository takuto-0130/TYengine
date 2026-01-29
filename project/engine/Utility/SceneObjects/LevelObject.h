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
    void SetModelName(const std::string& modelName) { modelName_ = modelName; }
    void SetPosition(Vector3 pos) { worldTransform_.SetTranslation(pos); }
    void SetRotation(Vector3 rota) { worldTransform_.SetRotate(rota); }
    void SetScale(Vector3 scale) { worldTransform_.SetScale(scale); }

private:
    std::string modelName_;
};

