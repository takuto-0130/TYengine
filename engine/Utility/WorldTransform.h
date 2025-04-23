#pragma once
#include "struct.h"
#include <d3d12.h>
#include <type_traits>
#include <wrl.h>

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Matrix4x4 WorldInverseTranspose;
};

/// <summary>
/// ワールド変換データ
/// </summary>
class WorldTransform {
public:
	// ローカルスケール
	Vector3 scale_ = { 1, 1, 1 };
	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation_ = { 0, 0, 0 };
	// ローカル座標
	Vector3 translation_ = { 0, 0, 0 };
	// ローカル → ワールド変換行列
	Matrix4x4 matWorld_;
	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	WorldTransform() = default;
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
	/// 定数バッファの取得
	/// </summary>
	/// <returns>定数バッファ</returns>
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetConstBuffer() const { return constBuffer_; }
	/// <summary>
	/// マップのセット
	/// </summary>
	/// <param name="wvp">WVP行列</param>
	void SetMapWVP(const Matrix4x4& wvp) { constMap->WVP = wvp; }
	void SetMapWorld(const Matrix4x4& world) { constMap->World = world; }
	const Matrix4x4& GetMatWorld() { return matWorld_; }

private:
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	TransformationMatrix* constMap = nullptr;
	// コピー禁止
	WorldTransform(const WorldTransform&) = delete;
	WorldTransform& operator=(const WorldTransform&) = delete;
};

static_assert(!std::is_copy_assignable_v<WorldTransform>);

