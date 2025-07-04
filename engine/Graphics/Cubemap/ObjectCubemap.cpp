#include "ObjectCubemap.h"
#include "mathFunc.h"
#include "operatorOverload.h"

void ObjectCubemap::Initialize(const std::string& textureFilePath)
{
    CreateCameraResource();
    CreateVertexData();
    CreateMaterialResource();
    CreateTransformationMatrixResource();

    textureFilePath_ = textureFilePath;
    TextureManager::GetInstance()->LoadTexture(textureFilePath);
}

void ObjectCubemap::Update()
{
    if (camera_) 
    {
        Matrix4x4 view = camera_->GetViewMatrix();
        // カメラの位置成分（平行移動）を取り除く
        view.m[3][0] = 0.0f;
        view.m[3][1] = 0.0f;
        view.m[3][2] = 0.0f;
        Matrix4x4 vp = view * camera_->GetProjectionMatrix();
        transformationMatrixData_->WVP = transformationMatrixData_->World * vp;
    }
    else
    {
        Matrix4x4 view = CubemapBasis::GetInstance()->GetDefaultCamera()->GetViewMatrix();
        // カメラの位置成分（平行移動）を取り除く
        view.m[3][0] = 0.0f;
        view.m[3][1] = 0.0f;
        view.m[3][2] = 0.0f;
        Matrix4x4 vp = view * CubemapBasis::GetInstance()->GetDefaultCamera()->GetProjectionMatrix();
        transformationMatrixData_->WVP = transformationMatrixData_->World * vp;
    }
}

void ObjectCubemap::Draw()
{
    // カメラデータ更新
    if (camera_) 
    {
        cameraData_->worldPosition = camera_->GetTranslate();
    }
    else
    {
        cameraData_->worldPosition = CubemapBasis::GetInstance()->GetDefaultCamera()->GetTranslate();
    }

    auto* commandList = CubemapBasis::GetInstance()->GetDirectXBasis()->GetCommandList();

    // Camera用CBVを設定（slot番号はルートシグネチャと合わせる）
    commandList->SetGraphicsRootConstantBufferView(3, cameraResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->IASetIndexBuffer(&indexBufferView_);
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));
    commandList->DrawIndexedInstanced(indexNum, 1, 0, 0, 0);
}

void ObjectCubemap::CreateVertexData()
{
    // リソースの作成
    vertexResource_ = CubemapBasis::GetInstance()->GetDirectXBasis()->CreateBufferResource(sizeof(CubeVertex) * 8);
    indexResource_ = CubemapBasis::GetInstance()->GetDirectXBasis()->CreateBufferResource(sizeof(uint32_t) * 36);

    // バッファビューの作成
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(CubeVertex) * 8; // 使用するリソースのサイズは頂点データ8個分のサイズ
    vertexBufferView_.StrideInBytes = sizeof(CubeVertex); // 1頂点当たりのサイズ

    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 36; // 使用するリソースのサイズはインデックス36個分のサイズ
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT; // インデックスはuint32_tとする

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    // 手前
    // 左下
    vertexData_[0].position = { -1.0f, -1.0f, -1.0f, 1.0f };
    // 左上
    vertexData_[1].position = { -1.0f, 1.0f, -1.0f, 1.0f };
    // 右下
    vertexData_[2].position = { 1.0f, -1.0f, -1.0f, 1.0f };
    // 右上
    vertexData_[3].position = { 1.0f, 1.0f, -1.0f, 1.0f };

    // 奥
    // 左下
    vertexData_[4].position = { -1.0f, -1.0f, 1.0f, 1.0f };
    // 左上
    vertexData_[5].position = { -1.0f, 1.0f, 1.0f, 1.0f };
    // 右下
    vertexData_[6].position = { 1.0f, -1.0f, 1.0f, 1.0f };
    // 右上
    vertexData_[7].position = { 1.0f, 1.0f, 1.0f, 1.0f };

    // 書き込むためのアドレスを取得
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
    static const uint32_t indices[] = {
        // 前面
        0, 1, 2, 1, 3, 2,
        // 後面
        6, 7, 4, 7, 5, 4,
        // 左面
        4, 5, 0, 5, 1, 0,
        // 右面
        2, 3, 6, 3, 7, 6,
        // 上面
        1, 5, 3, 5, 7, 3,
        // 下面
        4, 0, 6, 0, 2, 6,
    };

    memcpy(indexData_, indices, sizeof(indices));
}

void ObjectCubemap::CreateCameraResource()
{
    cameraResource_ = CubemapBasis::GetInstance()->GetDirectXBasis()->CreateBufferResource(sizeof(CameraForGPU));
    cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
    cameraData_->worldPosition = { 0.0f, 0.0f, 0.0f };
}

void ObjectCubemap::CreateMaterialResource()
{
    // マテリアル用のリソースを作る。今回はMaterial1つ分のサイズを用意する
    materialResource_ = CubemapBasis::GetInstance()->GetDirectXBasis()->CreateBufferResource(sizeof(Material));
    // 書き込むためのアドレスを取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    // 白を入れる
    materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void ObjectCubemap::CreateTransformationMatrixResource()
{
    transformationMatrixResource_ = CubemapBasis::GetInstance()->GetDirectXBasis()->CreateBufferResource(sizeof(CubeTransfomationMatrix));
    transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
    // 単位行列を書き込んでおく
    transformationMatrixData_->WVP = MakeIdentity4x4();
    transformationMatrixData_->World = MakeIdentity4x4();
    transformationMatrixData_->WorldInverseTranspose = MakeIdentity4x4();
}