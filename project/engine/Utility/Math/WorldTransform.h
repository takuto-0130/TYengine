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
public:
	/// <summary>コンストラクタ。</summary>
	WorldTransform() = default;
	/// <summary>デストラクタ。</summary>
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

	/// <summary>定数バッファへWVP行列を設定する。</summary>
	void SetMapWVP(const Matrix4x4& wvp) { constMap->WVP = wvp; }
	/// <summary>定数バッファへWorld行列を設定する。</summary>
	void SetMapWorld(const Matrix4x4& world) { constMap->World = world; }
	/// <summary>ローカルスケールを設定する。</summary>
	void SetScale(const Vector3& scale) { scale_ = scale; }
	/// <summary>ローカル回転（オイラー角）を設定する。</summary>
	void SetRotate(const Vector3& rotate) { rotation_ = rotate; }
	/// <summary>ローカル座標を設定する。</summary>
	void SetTranslation(const Vector3& translation) { translation_ = translation; }
	/// <summary>ローカル回転（クォータニオン）を設定する。</summary>
	void SetRotateQuaternion(const Quaternion& rotationQ) { rotationQ_ = rotationQ; }
	/// <summary>クォータニオンによる回転を使用するか設定する。</summary>
	void SetUseQuaternion(bool use) { useQuaternion_ = use; }
	/// <summary>親となるワールド変換を設定する（親子関係構築）。</summary>
	void SetParentWT(const WorldTransform* parent) { parent_ = parent; }
	/// <summary>親行列を直接設定する。</summary>
	void SetParentMatrix(const Matrix4x4* parentMatrix) { parentMatrix_ = parentMatrix; }


	// ========================
	//        Getter
	// ========================

	/// <summary>定数バッファのリソースを取得。</summary>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
	/// <summary>ワールド行列を取得。</summary>
	const Matrix4x4& GetMatWorld() { return matWorld_; }
	/// <summary>ローカルスケールを取得。</summary>
	const Vector3& GetScale() { return scale_; }
	/// <summary>ローカル回転（オイラー角）を取得。</summary>
	const Vector3& GetRotate() { return rotation_; }
	/// <summary>ローカル座標を取得。</summary>
	const Vector3& GetTranslation() { return translation_; }
	/// <summary>ローカル回転（クォータニオン）を取得。</summary>
	const Quaternion& GetRotateQuaternion() { return rotationQ_; }
	/// <summary>クォータニオン使用フラグを取得。</summary>
	bool GetUseQuaternion() { return useQuaternion_; }
	/// <summary>親のワールド変換情報を取得。</summary>
	const WorldTransform* GetParentWT() { return parent_; }
	/// <summary>親行列ポインタを取得。</summary>
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


	/// <summary>定数バッファ（GPUリソース）。</summary>
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	/// <summary>マップ済みアドレス。</summary>
	TransformationMatrix* constMap = nullptr;
	
	// コピー禁止
	WorldTransform(const WorldTransform&) = delete;
	WorldTransform& operator=(const WorldTransform&) = delete;
};

static_assert(!std::is_copy_assignable_v<WorldTransform>);

