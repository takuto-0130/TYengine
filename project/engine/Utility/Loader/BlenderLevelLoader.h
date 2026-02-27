#pragma once
#include "struct.h"
#include <string>
#include <json.hpp>

namespace TYEngine
{
	namespace Utility
	{

		class LevelObject;

		/// <summary>
		/// レベルデータ定義構造体。
		/// Blender等でエクスポートされたオブジェクト配置情報を保持する。
		/// </summary>
		struct LevelData
		{
			/// <summary>
			/// 単一のオブジェクトデータ。
			/// </summary>
			struct ObjectData
			{
				/// <summary>モデルファイル名（拡張子含む）。</summary>
				std::string fileName = "";
				/// <summary>平行移動（位置）。</summary>
				Vector3 translation = {};
				/// <summary>回転（オイラー角）。</summary>
				Vector3 rotation = {};
				/// <summary>スケール（拡大縮小）。</summary>
				Vector3 scaling = {};
				/// <summary>コライダーの中心オフセット。</summary>
				Vector3 colliderCenter = {};
				/// <summary>コライダーのサイズ（XYZ）。</summary>
				Vector3 colliderSize = {};
			};

			/// <summary>読み込まれたオブジェクトのリスト。</summary>
			std::vector<ObjectData> objects;
		};

		/// <summary>
		/// Blenderで作成したレベルエディタの配置データ（JSON形式）を読み込むクラス。
		/// 再帰的なオブジェクト走査にも対応。
		/// </summary>
		class BlenderLevelLoader
		{
		public:
			/// <summary>
			/// コンストラクタ。
			/// </summary>
			/// <param name="directoryPath">JSONファイルのベースディレクトリパス。</param>
			BlenderLevelLoader(const std::string& directoryPath = "Resources/JSON/") : kBaseDirectoryName_(directoryPath) {}

			/// <summary>
			/// 指定されたJSONファイルからレベルデータを読み込む。
			/// </summary>
			/// <param name="filename">JSONファイルのパス。</param>
			/// <returns>読み込まれたレベルデータへのポインタ。</returns>
			LevelData* Load(const std::string& filename);

			/// <summary>
			/// ロードしたレベルデータを基に、実際のゲームオブジェクトを生成・配置する。
			/// </summary>
			/// <param name="data">読み込み済みのレベルデータ。</param>
			/// <param name="objects">生成されたオブジェクトを格納する配列。</param>
			void DataToObject(LevelData* data, std::vector<std::unique_ptr<LevelObject>>& objects);

		private:
			// オブジェクト走査
			void ObjectTraversal(LevelData* levelData, nlohmann::json& j, std::string contains);

		private:
			/// <summary>JSONファイルのベースディレクトリパス。</summary>
			std::string kBaseDirectoryName_;

			/// <summary>ロードされたレベルデータ。</summary>
			std::unique_ptr<LevelData> levelData_;
		};

	} // namespace Utility
} // namespace TYEngine

