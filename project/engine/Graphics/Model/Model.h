#pragma once
#include "ModelLoader.h"
#include "struct.h"
#include "Matrix4x4Func.h"
#include <fstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Camera.h>


namespace TYEngine
{
	namespace Utility { class WorldTransform; }

	namespace Graphics
	{

		class ModelLoader;

		/// <summary>
		/// 単一の 3D モデルを表すクラス。  
		/// モデルデータ（頂点・マテリアル・ノード階層）を保持し、描画処理を提供する。
		/// </summary>
		class Model
		{
		public: // メンバ関数
			/// <summary>
			/// モデルの初期化処理。  
			/// 指定ディレクトリからモデルファイルを読み込み、頂点データやマテリアル情報を構築する。
			/// </summary>
			/// <param name="modelLoader">モデル読み込み管理クラス。</param>
			/// <param name="directoryPath">モデルのディレクトリパス。</param>
			/// <param name="fileName">モデルファイル名。</param>
			void Initialize(ModelLoader* modelLoader, const std::string& directoryPath, const std::string& fileName);

			/// <summary>
			/// モデルの描画処理。  
			/// ワールド変換およびカメラ情報を使用してレンダリングを行う。
			/// </summary>
			/// <param name="transform">オブジェクトのワールド変換情報。</param>
			/// <param name="camera">描画に使用するカメラ。</param>
			void Draw(Utility::WorldTransform& transform, CameraSystem::Camera* camera);

			/// <summary>
			/// モデルの頂点データから頂点バッファを生成する。  
			/// GPU 上にリソースを確保し、頂点ビューを構築する。
			/// </summary>
			void CreateVertexResource();

			/// <summary>
			/// 頂点データ構造体。  
			/// 各頂点の座標、UV 座標、法線ベクトルを保持する。
			/// </summary>
			struct VertexData
			{
				Utility::Vector4 position;  ///< 頂点座標。
				Utility::Vector2 texCoord;  ///< テクスチャ座標。
				Utility::Vector3 normal;    ///< 法線ベクトル。
			};
		private: // 構造体

			/// <summary>
			/// RGB カラー構造体。
			/// </summary>
			struct Color
			{
				float r, g, b;
			};

			/// <summary>
			/// マテリアルデータ構造体。  
			/// 拡散・反射・環境光情報、テクスチャファイルパスなどを保持する。
			/// </summary>
			struct MaterialData
			{
				std::string name;           ///< マテリアル名。
				float Ns;                   ///< スペキュラ強度。
				Color Ka;                   ///< 環境光色。
				Color Kd;                   ///< 拡散反射色。
				Color Ks;                   ///< 鏡面反射色。
				float Ni;                   ///< 屈折率。
				float d;                    ///< 透過度。
				uint32_t illum;             ///< 照明モデル。
				std::string textureFilePath;///< テクスチャファイルパス。
				uint32_t textureIndex = 0;  ///< テクスチャ SRV インデックス。
			};

			/// <summary>
			/// モデル階層構造を表すノード。
			/// 子ノードを持つことができ、階層的なモデル構成を再現する。
			/// </summary>
			struct Node
			{
				Utility::Matrix4x4 localMatrix;          ///< ローカル変換行列。
				std::string name;               ///< ノード名。
				std::vector<Node> children;     ///< 子ノード群。
			};

			/// <summary>
			/// モデル全体のデータ構造体。  
			/// 頂点配列、マテリアル、ルートノードをまとめる。
			/// </summary>
			struct ModelData
			{
				std::vector<VertexData> vertices; ///< 頂点データ配列。
				MaterialData material;            ///< マテリアル情報。
				Node rootNode;                    ///< ルートノード。
			};

		private: // メンバ変数
			ModelLoader* modelLoader_ = nullptr; ///< モデルローダへの参照。
			ModelData modelData_;                ///< モデルデータ本体。

			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr; ///< 頂点バッファリソース。
			VertexData* vertexData_ = nullptr;                                ///< 頂点データマッピングポインタ。
			D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;                       ///< 頂点バッファビュー。

		public:
			/// <summary>
			/// OBJ モデルファイルを読み込む。  
			/// Assimp などを使用してデータを解析し、ModelData 構造体として返す。
			/// </summary>
			/// <param name="directoryPath">モデルのディレクトリパス。</param>
			/// <param name="filename">読み込むファイル名。</param>
			/// <returns>ロード済みモデルデータ。</returns>
			static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

			/// <summary>
			/// ノード階層情報を再帰的に解析し、Node 構造体に変換する。
			/// </summary>
			/// <param name="node">Assimp のノード構造体。</param>
			/// <returns>変換後の Node 構造体。</returns>
			static Node ReadNode(aiNode* node);

			/// <summary>
			/// プログラム上で動的に生成した頂点データからモデルを初期化する
			/// </summary>
			void InitializeDynamic(ModelLoader* modelLoader, const std::vector<VertexData>& vertices, const std::string& textureFilePath);


		public:
			/// <summary>
			/// 頂点データを追加する。
			/// </summary>
			/// <param name="vertex">追加する頂点情報。</param>
			void SetVertices(VertexData vertex);

			/// <summary>
			/// 使用するテクスチャファイルのパスを設定する。
			/// </summary>
			/// <param name="filePath">テクスチャファイルへのパス。</param>
			void SetTexturePath(const std::string& filePath) { modelData_.material.textureFilePath = filePath; }
		};

	} // namespace Graphics
} // namespace TYEngine
