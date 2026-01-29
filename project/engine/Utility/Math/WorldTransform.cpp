#include "WorldTransform.h"
#include "Matrix4x4Func.h"
#include "DirectXBasis.h"
#include "Object3dBasis.h"

void WorldTransform::Initialize()
{
    // ワールド行列の初期化
    matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

    // 定数バッファ生成
    CreateConstBuffer();

    // 定数バッファへ初期行列を転送
    Update();
}

void WorldTransform::CreateConstBuffer()
{
    // MVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
    constBuffer_ = Object3dBasis::GetInstance()->GetDirectXBasis()->CreateBufferResource(sizeof(TransformationMatrix));
    // 書き込むためのアドレスを取得
    constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
    // 単位行列を書き込んでおく
    constMap->WVP = MakeIdentity4x4();
    constMap->World = MakeIdentity4x4();
    constMap->WorldInverseTranspose = MakeIdentity4x4();
}

void WorldTransform::TransferMatrix()
{
    // ワールド行列を定数バッファに転送
    if (constMap != nullptr)
    {
        constMap->World = matWorld_; // 定数バッファに行列をコピー
        constMap->WorldInverseTranspose = Inverse(matWorld_);
    }
}

void WorldTransform::UpdateMatrix()
{
    // スケール、回転、平行移動を合成して行列を計算する
    if (useQuaternion_)
    {
        matWorld_ = MakeAffineMatrix(scale_, rotationQ_, translation_);
    }
    else
    {
        matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
    }

    // 親が存在する場合、親のワールド行列を掛け合わせる(parent_ と parentMatrix_ 両方に値が入っていた場合　parent_ を優先)
    if (parent_)
    {
        
        matWorld_ = matWorld_ * parent_->matWorld_; // 親の行列と自身の行列を合成
    }
    else if (parentMatrix_ != nullptr)
    {
        matWorld_ = matWorld_ * (*parentMatrix_);
    }
}

void WorldTransform::Update()
{
    UpdateMatrix();
    TransferMatrix();
}
