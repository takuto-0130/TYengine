#pragma once
#include "struct.h"
#include "Matrix4x4Func.h"
#include "Quaternion.h"
#include <d3d12.h>
#include <type_traits>
#include <wrl.h>

/// <summary>
/// ワールド変換データ
/// </summary>
class WorldTransform
{
public:
	// コンストラクタ
	WorldTransform() = default;
	// デストラクタ
	~WorldTransform() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 行列の転送・更新をする
	/// </summary>
	void Update();


	// ========================
	//        Setter
	// ========================

	void SetMapWVP(const Matrix4x4& wvp) { constMap->WVP = wvp; }
	void SetMapWorld(const Matrix4x4& world) { constMap->World = world; }
	void SetScale(const Vector3& scale) { scale_ = scale; }
	void SetRotate(const Vector3& rotate) { rotation_ = rotate; }
	void SetTranslation(const Vector3& translation) { translation_ = translation; }
	void SetRotateQuaternion(const Quaternion& rotationQ) { rotationQ_ = rotationQ; }
	void SetUseQuaternion(bool use) { useQuaternion_ = use; }
	void SetParentWT(const WorldTransform* parent) { parent_ = parent; }
	void SetParentMatrix(const Matrix4x4* parentMatrix) { parentMatrix_ = parentMatrix; }


	// ========================
	//        Getter
	// ========================

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
	const Matrix4x4& GetMatWorld() { return matWorld_; }
	const Vector3& GetScale() { return scale_; }
	const Vector3& GetRotate() { return rotation_; }
	const Vector3& GetTranslation() { return translation_; }
	const Quaternion& GetRotateQuaternion() { return rotationQ_; }
	bool GetUseQuaternion() { return useQuaternion_; }
	const WorldTransform* GetParentWT() { return parent_; }
	const Matrix4x4* GetParentMatrix() { return parentMatrix_; }

private:
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInverseTranspose;
	};

	// ローカルスケール
	Vector3 scale_ = { 1, 1, 1 };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0, 0, 0 };
	// クォータニオン
	Quaternion rotationQ_{};
	bool useQuaternion_ = false; // 使用するかどうか
	// ローカル座標
	Vector3 translation_ = { 0, 0, 0 };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_ = MakeIdentity4x4();
	// 親となるワールド変換（階層構造用）
	const WorldTransform* parent_ = nullptr;
	// 親となる行列への直接参照（特定の行列に追従させたい場合用）
	const Matrix4x4* parentMatrix_ = nullptr;


	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	TransformationMatrix* constMap = nullptr;
	// コピー禁止
	WorldTransform(const WorldTransform&) = delete;
	WorldTransform& operator=(const WorldTransform&) = delete;
};

static_assert(!std::is_copy_assignable_v<WorldTransform>);

