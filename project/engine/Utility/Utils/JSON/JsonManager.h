#pragma once

//
// JsonManager.hpp — Completely integrated JSON manager
//
// 階層化されたJSONデータ（nlohmann::jsonを使用）をファイルシステムと連携して管理するためのクラス。
// パスベースの読み書き、スキーマバージョン管理、自動保存・バックアップ機能、
// および ImGui 上での完全な JSON エディタ機能を提供する。
//
//  - Path-based read/write: ドット(.)やブラケット([])を用いたパスによるデータアクセス
//  - Autosave & backup: 自動保存および保存時のバックアップ生成
//  - Schema / Migration: データ構造のバージョン管理とマイグレーション機能
//  - Full ImGui JSON Editor: ImGui 上での JSON 構造の編集インターフェース
//  - Type Combo: プリミティブ型に加え、Vector2/3/4 型の判別と編集に対応
//

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

#include <nlohmann/json.hpp>

#include "struct.h"

namespace TYEngine {
namespace Utility {

{

    using json = nlohmann::json;

    // ==========================
    // Path structure
    // ==========================

    /// <summary>
    /// JSON パス（例: "root.array[0].key"）を構成する要素（トークン）を表す構造体。
    /// </summary>
    struct PathToken
    {
        enum class Kind { Key, Index }; ///< トークンの種類: Key（オブジェクトのキー）または Index（配列のインデックス）。
        Kind kind{};					///< トークンの種類。
        std::string key;				///< Kind が Key の場合に使用されるキー文字列。
        size_t index{};					///< Kind が Index の場合に使用される配列インデックス。
    };

    /// <summary>
    /// JSON データへのアクセスパスを表現・解析するクラス。
    /// </summary>
    class Path
    {
    public:
        std::vector<PathToken> tokens; ///< パスを構成するトークンのリスト。

        /// <summary>
        /// 文字列形式のパスを解析し、トークンのリストに変換する。
        /// </summary>
        /// <param name="text">解析するパス文字列（例: "objects[0].name"）。</param>
        /// <returns>解析された Path オブジェクト。</returns>
        /// <exception cref="std::runtime_error">インデックスの解析に失敗した場合。</exception>
        static Path Parse(std::string_view text)
        {
            Path p;
            std::string cur;

            auto flush_key = [&]()
                {
                    if (!cur.empty())
                    {
                        p.tokens.push_back(PathToken{ PathToken::Kind::Key, cur, 0 });
                        cur.clear();
                    }
                };

            for (size_t i = 0; i < text.size(); ++i)
            {
                char c = text[i];

                if (c == '.')
                {
                    flush_key();
                }
                else if (c == '[')
                {
                    flush_key();
                    size_t j = i + 1;
                    size_t val = 0;
                    bool ok = false;

                    for (; j < text.size() && text[j] != ']'; ++j)
                    {
                        if (text[j] >= '0' && text[j] <= '9')
                        {
                            ok = true;
                            val = val * 10 + (text[j] - '0');
                        }
                        else
                        {
                            ok = false;
                            break;
                        }
                    }

                    if (!ok || j >= text.size() || text[j] != ']')
                    {
                        throw std::runtime_error("Invalid index in path: " + std::string(text));
                    }

                    p.tokens.push_back(PathToken{ PathToken::Kind::Index, {}, val });
                    i = j;
                }
                else
                {
                    cur.push_back(c);
                }
            }
            flush_key();
            return p;
        }
    };

    // ==========================
    // JsonManager Core
    // ==========================

    /// <summary>
    /// JSONデータのロード、保存、スキーマ管理、パスアクセスを提供するコアマネージャクラス。
    /// </summary>
    class JsonManager
    {
    public:
        /// <summary>JSONデータのバリデーション（検証）を行うための関数型。</summary>
        using ValidateFn = std::function<bool(json& root, std::string& out_msg)>;
        /// <summary>JSONデータのマイグレーション（構造移行）を行うための関数型。</summary>
        using MigrateFn = std::function<void(int from_version, json& root)>;

        /// <summary>
        /// JsonManager の動作オプションを設定する構造体。
        /// </summary>
        struct Options
        {
            int   pretty_indent = 2;			            ///< JSONファイル書き出し時のインデント数。0以下の場合はミニファイ（圧縮）される。
            bool  autosave = false;				            ///< MarkDirty 時に自動で保存を行うかどうか。
            bool  make_backup_on_save = true;	            ///< Save 時に .bak ファイルを作成するかどうか。
            std::string meta_key = "_meta";		            ///< スキーマバージョンなどのメタ情報を格納するオブジェクトのキー名。
            std::string version_key = "schema";	            ///< メタ情報内でスキーマバージョンを格納するキー名。
            std::string directory_path = "Resources/JSON";
        };

    public:
        JsonManager() = default;
        /// <summary>
        /// オプションを指定して JsonManager を構築する。
        /// </summary>
        /// <param name="o">設定オプション。</param>
        explicit JsonManager(Options o) : opt_(o) {}

        // --------------------------
        // Load
        // --------------------------
        /// <summary>
        /// 指定されたファイルから JSON データをロードする。
        /// </summary>
        /// <param name="file">ロードするファイルのパス。</param>
        /// <param name="create_if_missing">ファイルが存在しない場合に空の JSON オブジェクトを作成して保存するかどうか。</param>
        /// <param name="err">ロードまたは作成に失敗した場合のエラーメッセージを格納するポインタ。</param>
        /// <returns>ロードまたは作成に成功した場合に true。</returns>
        bool Load(const std::string& file, bool create_if_missing = true, std::string* err = nullptr)
        {
            
            last_path_ = std::filesystem::path(opt_.directory_path.c_str()) / file.c_str();

            if (!std::filesystem::exists(last_path_))
            {
                if (!create_if_missing)
                {
                    if (err) *err = "file not found";
                    return false;
                }
                root_ = json::object();
                dirty_ = true;
                return Save(last_path_, err); // 新規ファイルとして保存
            }

            try
            {
                std::ifstream ifs(last_path_, std::ios::binary);
                if (!ifs)
                {
                    if (err) *err = "failed to open";
                    return false;
                }
                root_ = json::parse(ifs, nullptr, true, true);
                dirty_ = false;
                return true;
            }
            catch (const std::exception& e)
            {
                if (err) *err = e.what();
                return false;
            }
        }

        // --------------------------
        // Save
        // --------------------------
        /// <summary>
        /// 現在の JSON データをファイルに保存する。
        /// </summary>
        /// <param name="file">保存先のファイルのパス。省略した場合、最後に Load/Save したパスを使用する。</param>
        /// <param name="err">保存に失敗した場合のエラーメッセージを格納するポインタ。</param>
        /// <returns>保存に成功した場合に true。</returns>
        bool Save(std::optional<std::filesystem::path> file = std::nullopt, std::string* err = nullptr)
        {
            const auto& path = file ? *file : last_path_;
            if (path.empty())
            {
                if (err) *err = "no path";
                return false;
            }

            try
            {
                // バックアップ作成オプションが有効で、ファイルが存在する場合
                if (opt_.make_backup_on_save && std::filesystem::exists(path))
                {
                    std::filesystem::path bak = path;
                    bak += ".bak";
                    std::error_code ec;
                    // ファイルをコピーしてバックアップを作成
                    std::filesystem::copy_file(path, bak, std::filesystem::copy_options::overwrite_existing, ec);
                }

                std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
                if (!ofs)
                {
                    if (err) *err = "failed to write";
                    return false;
                }
                // インデント設定に従ってダンプ（書き出し）
                if (opt_.pretty_indent > 0)
                    ofs << root_.dump(opt_.pretty_indent);
                else
                    ofs << root_.dump();

                dirty_ = false;
                last_path_ = path;
                return true;
            }
            catch (const std::exception& e)
            {
                if (err) *err = e.what();
                return false;
            }
        }

        // --------------------------
        // Schema
        // --------------------------
        /// <summary>
        /// 現在の JSON データに設定されているスキーマバージョンを取得する。
        /// メタ情報がない、または不正な場合は 0 を返す。
        /// </summary>
        /// <returns>スキーマバージョン。</returns>
        int GetSchemaVersion() const
        {
            if (!root_.contains(opt_.meta_key)) return 0;
            const json& m = root_[opt_.meta_key];
            if (m.is_object() && m.contains(opt_.version_key) && m[opt_.version_key].is_number_integer())
                return m[opt_.version_key].get<int>();
            return 0;
        }

        /// <summary>
        /// JSON データにスキーマバージョンを設定し、ダーティフラグを立てる。
        /// </summary>
        /// <param name="v">設定するバージョン番号。</param>
        void SetSchemaVersion(int v)
        {
            root_[opt_.meta_key][opt_.version_key] = v;
            MarkDirty();
        }

        /// <summary>
        /// データ検証を行うバリデーター関数を設定する。
        /// </summary>
        /// <param name="v">バリデーター関数。</param>
        void SetValidator(ValidateFn v) { validator_ = std::move(v); }
        /// <summary>
        /// データ構造の移行を行うマイグレーター関数を設定する。
        /// </summary>
        /// <param name="m">マイグレーター関数。</param>
        void SetMigrator(MigrateFn  m) { migrator_ = std::move(m); }

        /// <summary>
        /// スキーマバージョンに基づいてマイグレーションを実行し、バリデーションを行う。
        /// </summary>
        /// <param name="target_version">マイグレーションの目標バージョン。現在のバージョンより高い場合にマイグレーションが実行される。</param>
        /// <param name="err">エラーメッセージを格納するポインタ。</param>
        /// <returns>マイグレーションとバリデーションが成功した場合に true。</returns>
        bool ValidateAndMigrate(int target_version = -1, std::string* err = nullptr)
        {
            try
            {
                int cur = GetSchemaVersion();

                // マイグレーターが設定されており、目標バージョンが現在のバージョンより高い場合
                if (migrator_ && target_version >= 0 && cur < target_version)
                {
                    migrator_(cur, root_); // マイグレーションを実行
                    SetSchemaVersion(target_version);
                }

                // バリデーターが設定されている場合
                if (validator_)
                {
                    std::string msg;
                    if (!validator_(root_, msg))
                    {
                        if (err) *err = msg;
                        return false;
                    }
                }
                return true;
            }
            catch (const std::exception& e)
            {
                if (err) *err = e.what();
                return false;
            }
        }

        // --------------------------
        // Path accessors
        // --------------------------

        /// <summary>
        /// パスで指定された JSON ノードを検索し、存在すればそのポインタを返す。
        /// </summary>
        /// <param name="path">アクセスパス（例: "data.items[0]"）。</param>
        /// <returns>ノードのポインタ。ノードが存在しない場合は nullptr。</returns>
        json* Find(std::string_view path)
        {
            Path p = Path::Parse(path);
            json* n = &root_; // ルートノードから開始

            for (auto& t : p.tokens)
            {
                if (t.kind == PathToken::Kind::Key)
                {
                    if (!n->is_object()) return nullptr; // オブジェクトではない -> キーアクセス不可
                    auto it = n->find(t.key);
                    if (it == n->end()) return nullptr;  // キーが見つからない
                    n = &(*it);
                }
                else
                {
                    if (!n->is_array()) return nullptr;	// 配列ではない -> インデックスアクセス不可
                    if (t.index >= n->size()) return nullptr; // インデックスが範囲外
                    n = &(*n)[t.index];
                }
            }
            return n;
        }

        /// <summary>
        /// パスで指定された JSON ノードを検索し、存在しない場合は作成してそのノードの参照を返す。
        /// 途中のノードが不正な型（例: 文字列ノードをオブジェクトとしてアクセス）の場合、型を上書きして作成する。
        /// </summary>
        /// <param name="path">アクセスパス（例: "data.items[0]"）。</param>
        /// <returns>ノードの参照。</returns>
        json& Ensure(std::string_view path)
        {
            Path p = Path::Parse(path);
            json* n = &root_;

            for (auto& t : p.tokens)
            {
                if (t.kind == PathToken::Kind::Key)
                {
                    if (!n->is_object()) *n = json::object(); // 現在ノードがオブジェクトでなければオブジェクト化
                    n = &(*n)[t.key];
                }
                else
                {
                    if (!n->is_array()) *n = json::array(); // 現在ノードが配列でなければ配列化
                    auto& arr = *n;
                    // インデックスに到達するまで null 要素で埋める
                    while (arr.size() <= t.index) arr.push_back(json());
                    n = &arr[t.index];
                }
            }
            return *n;
        }

        /// <summary>
        /// パスで指定されたノードの値を指定された型で取得する。
        /// </summary>
        /// <typeparam name="T">取得するデータの型。</typeparam>
        /// <param name="path">アクセスパス。</param>
        /// <param name="def_val">ノードが存在しない、または型変換に失敗した場合に返すデフォルト値。</param>
        /// <returns>ノードの値、またはデフォルト値。</returns>
        template<typename T>
        T Get(std::string_view path, T def_val = {}) const
        {
            const json* n = const_cast<JsonManager*>(this)->Find(path);
            if (!n) return def_val;
            try { return n->get<T>(); }
            catch (...) { return def_val; }
        }

        /// <summary>
        /// パスで指定されたノードに値を設定する。ノードが存在しない場合は作成される（Ensure相当）。
        /// </summary>
        /// <typeparam name="T">設定するデータの型。</typeparam>
        /// <param name="path">アクセスパス。</param>
        /// <param name="v">設定する値。</param>
        /// <returns>設定に成功した場合に true。</returns>
        template<typename T>
        bool Set(std::string_view path, const T& v)
        {
            try
            {
                json& n = Ensure(path); // ノードを確実に取得/作成
                n = v;
                MarkDirty(); // データ変更をマーク
                return true;
            }
            catch (...) { return false; }
        }

        /// <summary>
        /// パスで指定されたノードを親オブジェクトまたは配列から削除する。
        /// </summary>
        /// <param name="path">削除対象のノードへのパス。</param>
        /// <returns>削除に成功した場合に true。</returns>
        bool Erase(std::string_view path)
        {
            Path p = Path::Parse(path);
            if (p.tokens.empty()) return false;

            // 削除対象の親ノードを見つける
            json* parent = &root_;
            for (size_t i = 0; i + 1 < p.tokens.size(); ++i)
            {
                auto& t = p.tokens[i];
                if (t.kind == PathToken::Kind::Key)
                {
                    if (!parent->is_object() || !parent->contains(t.key)) return false;
                    parent = &(*parent)[t.key];
                }
                else
                {
                    if (!parent->is_array() || t.index >= parent->size()) return false;
                    parent = &(*parent)[t.index];
                }
            }

            // 最後のトークンに基づいて削除を実行
            auto& last = p.tokens.back();
            if (last.kind == PathToken::Kind::Key)
            {
                if (parent->is_object())
                    return parent->erase(last.key) > 0;
            }
            else
            {
                if (parent->is_array() && last.index < parent->size())
                {
                    parent->erase(parent->begin() + last.index);
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// JSON データのルートノードへの参照を返す。
        /// </summary>
        json& Root() { return root_; }
        /// <summary>
        /// JSON データのルートノードへの const 参照を返す。
        /// </summary>
        const json& Root() const { return root_; }

        /// <summary>
        /// データが変更され、未保存の状態であるか（ダーティ状態）を取得する。
        /// </summary>
        /// <returns>ダーティ状態の場合 true。</returns>
        bool IsDirty() const { return dirty_; }
        /// <summary>
        /// ダーティフラグを設定する。オートセーブが有効な場合は保存処理も試行される。
        /// </summary>
        /// <param name="d">設定するダーティフラグの状態。</param>
        void MarkDirty(bool d = true)
        {
            dirty_ = d;
            if (dirty_) maybeAutosave();
        }

        /// <summary>
        /// 現在のオプション設定を取得する。
        /// </summary>
        const Options& GetOptions() const { return opt_; }
        /// <summary>
        /// オプション設定を上書きする。
        /// </summary>
        void SetOptions(const Options& o) { opt_ = o; }

        /// <summary>
        /// 最後にロードまたは保存を行ったファイルのパスを取得する。
        /// </summary>
        const std::filesystem::path& LastPath() const { return last_path_; }

    private:
        /// <summary>
        /// オートセーブオプションが有効な場合、設定されたパスに保存を試みる。
        /// </summary>
        void maybeAutosave()
        {
            if (opt_.autosave && !last_path_.empty())
            {
                std::string err;
                Save(last_path_, &err);
            }
        }

    private:
        json root_ = json::object();				///< JSON データのルートノード。
        bool dirty_ = false;						///< データが変更されたかどうかを示すフラグ（未保存状態）。

        std::filesystem::path last_path_;			///< 最後に操作したファイルのパス。
        Options opt_;								///< マネージャのオプション設定。

        ValidateFn validator_;						///< データ検証を行う関数。
        MigrateFn migrator_;						///< データ移行を行う関数。
    };



#ifdef JSONMGR_WITH_IMGUI
#include <imgui.h>
    // =========================================================
    //   Vector 判定
    // =========================================================

    /// <summary>
    /// JSONノードが Vector2 と見なせる構造を持つか判定する。
    /// （オブジェクトであり、"x", "y" という float 型のキーを持ち、サイズが 2 であること）
    /// </summary>
    inline bool isVector2(const json& j)
    {
        return j.is_object() &&
            j.contains("x") && j["x"].is_number_float() &&
            j.contains("y") && j["y"].is_number_float() &&
            j.size() == 2;
    }

    /// <summary>
    /// JSONノードが Vector3 と見なせる構造を持つか判定する。
    /// </summary>
    inline bool isVector3(const json& j)
    {
        return j.is_object() &&
            j.contains("x") && j["x"].is_number_float() &&
            j.contains("y") && j["y"].is_number_float() &&
            j.contains("z") && j["z"].is_number_float() &&
            j.size() == 3;
    }

    /// <summary>
    /// JSONノードが Vector4 と見なせる構造を持つか判定する。
    /// </summary>
    inline bool isVector4(const json& j)
    {
        return j.is_object() &&
            j.contains("x") && j["x"].is_number_float() &&
            j.contains("y") && j["y"].is_number_float() &&
            j.contains("z") && j["z"].is_number_float() &&
            j.contains("w") && j["w"].is_number_float() &&
            j.size() == 4;
    }

    // =========================================================
    //  InputTextDynamic（公式 CallbackResize 準拠）
    // =========================================================
    /// <summary>
    /// ImGui::InputText を std::string に対応させ、バッファを動的にリサイズする。
    /// </summary>
    inline bool InputTextDynamic(const char* label, std::string& str)
    {
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;

        // バッファ確保
        if (str.capacity() < 32)
            str.reserve(32);

        return ImGui::InputText(
            label,
            (char*)str.data(),
            str.capacity() + 1,
            flags,
            [](ImGuiInputTextCallbackData* data) -> int
            {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    std::string* s = (std::string*)data->UserData;
                    s->resize(data->BufTextLen);
                    data->Buf = (char*)s->data();
                }
                return 0;
			},
			(void*)&str
		);
	}

	// =========================================================
	//      JsonImGuiEditor — 完全統合版
	// =========================================================

	static const char* JSON_TYPES[] =
    {
	    "empty",
	    "string",
	    "int",
	    "float",
	    "bool",
	    "object",
	    "array",
	    "Vector2",
	    "Vector3",
	    "Vector4"
	};

	/// <summary>
	/// ImGui 上で JSON データを編集するためのインターフェースを提供するクラス。
	/// </summary>
	class JsonImGuiEditor
	{
	public:
		explicit JsonImGuiEditor(JsonManager& mgr) : mgr_(mgr) {}

		/// <summary>
		/// 指定された JSON ノードを ImGui ウィンドウに描画し、編集可能にする。
		/// </summary>
		/// <param name="node">編集対象の JSON ノード（通常はルートノード）。</param>
        /// <param name="label">ルートノードに表示するラベル。</param>
        void Draw(json& node, const char* label = "root")
        {
            ImGui::PushID(label);
            drawNode(label, node);
            ImGui::PopID();
        }

    private:

        // =========================================================
        //      型 Combo + 型変換
        // =========================================================

        /// <summary>
        /// JSON ノードの型を選択するためのコンボボックスを描画し、型の変更を処理する。
        /// </summary>
        void drawTypeSelector(json& node)
        {

            int cur = 0;

            if (node.is_object()) cur = 0;
            else if (node.is_string()) cur = 1;
            else if (node.is_number_integer()) cur = 2;
            else if (node.is_number_float()) cur = 3;
            else if (node.is_boolean()) cur = 4;
            else if (isVector2(node)) cur = 7;
            else if (isVector3(node)) cur = 8;
            else if (isVector4(node)) cur = 9;
            else if (node.is_object()) cur = 5;
            else if (node.is_array()) cur = 6;

            ImGui::SetNextItemWidth(100);
            if (ImGui::Combo("##type", &cur, JSON_TYPES, IM_ARRAYSIZE(JSON_TYPES)))
            {
                // 型変更に応じてノードの値を初期化
                switch (cur)
                {
                case 0:  node = json::object(); break;
                case 1:  node = std::string(""); break;
                case 2:  node = 0; break;
                case 3:  node = 0.0f; break;
                case 4:  node = false; break;
                case 5:  node = json::object(); break;
                case 6:  node = json::array(); break;
                case 7:  node = json{ {"x",0.0f},{"y",0.0f} }; break;
                case 8:  node = json{ {"x",0.0f},{"y",0.0f},{"z",0.0f} }; break;
                case 9:  node = json{ {"x",0.0f},{"y",0.0f},{"z",0.0f},{"w",0.0f} }; break;
                }
                mgr_.MarkDirty();
            }
        }

        // =========================================================
        // Vector 各種 Editor
        // =========================================================

        /// <summary>
        /// Vector2 ノードを ImGui::DragFloat2 として描画する。
        /// </summary>
        void drawVector2(const char* label, json& node)
        {
            float v[2] = { node["x"], node["y"] };
            if (ImGui::DragFloat2(label, v, 0.1f))
            {
                node["x"] = v[0];
                node["y"] = v[1];
                mgr_.MarkDirty();
            }
        }

        /// <summary>
        /// Vector3 ノードを ImGui::DragFloat3 として描画する。
        /// </summary>
        void drawVector3(const char* label, json& node)
        {
            float v[3] = { node["x"], node["y"], node["z"] };
            if (ImGui::DragFloat3(label, v, 0.1f))
            {
                node["x"] = v[0];
                node["y"] = v[1];
                node["z"] = v[2];
                mgr_.MarkDirty();
            }
        }

        /// <summary>
        /// Vector4 ノードを ImGui::DragFloat4 として描画する。
        /// </summary>
        void drawVector4(const char* label, json& node)
        {
            float v[4] = { node["x"], node["y"], node["z"], node["w"] };
            if (ImGui::DragFloat4(label, v, 0.1f))
            {
                node["x"] = v[0];
                node["y"] = v[1];
                node["z"] = v[2];
                node["w"] = v[3];
                mgr_.MarkDirty();
            }
        }

        // =========================================================
        // Node Dispatcher
        // =========================================================

        /// <summary>
        /// JSON ノードの型に基づいて、適切な描画関数に処理を振り分ける。
        /// </summary>
        void drawNode(const char* label, json& node)
        {
            // まず Vector 型を優先的に判定
            if (isVector2(node)) { drawVector2(label, node); return; }
            if (isVector3(node)) { drawVector3(label, node); return; }
            if (isVector4(node)) { drawVector4(label, node); return; }

            using Type = json::value_t;
            Type t = node.type();

            // JSON プリミティブ型、構造型に振り分け
            switch (t)
            {
            case Type::object: drawObject(label, node); break;
            case Type::array:  drawArray(label, node); break;
            case Type::string: drawString(label, node); break;
            case Type::number_integer: drawInt(label, node); break;
            case Type::number_float: drawFloat(label, node); break;
            case Type::boolean: drawBool(label, node); break;
            case Type::null: drawNull(); break;
            default:
                ImGui::TextUnformatted("(unsupported)");
                break;
            }
        }

        // =========================================================
        // object editor
        // =========================================================

        std::unordered_map<ImGuiID, std::string> addKeyMap; ///< 新しいキー入力用のマップ
        std::unordered_map<ImGuiID, int> addTypeMap;		///< 新しい値の型選択用のマップ

        /// <summary>
        /// 選択された型に基づいて新しい JSON ノードの初期値を作成する。
        /// </summary>
        json createValueByType(int t)
        {
            switch (t)
            {
            case 0:  return json::object(); break;
            case 1:  return std::string(""); break;
            case 2:  return 0; break;
            case 3:  return 0.0f; break;
            case 4:  return false; break;
            case 5:  return json::object(); break;
            case 6:  return json::array(); break;
            case 7:  return json{ {"x",0.0f},{"y",0.0f} }; break;
            case 8:  return json{ {"x",0.0f},{"y",0.0f},{"z",0.0f} }; break;
            case 9:  return json{ {"x",0.0f},{"y",0.0f},{"z",0.0f},{"w",0.0f} }; break;
            }
            return json::object();
        }


        /// <summary>
        /// JSON オブジェクトノードを ImGui に描画し、キーと値の編集、新規キーの追加、要素の削除を可能にする。
        /// </summary>
        void drawObject(const char* label, json& obj)
        {
            if (!ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
                return;

            ImGuiID id = ImGui::GetID(label);
            std::string& newKey = addKeyMap[id];
            int& newType = addTypeMap[id];

            ImGui::Separator();

            // ---- 新規キーの追加インターフェース ----
            ImGui::Text("AddKey:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            InputTextDynamic("##newKey", newKey);

            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::Combo("##type", &newType, JSON_TYPES, IM_ARRAYSIZE(JSON_TYPES));

            ImGui::SameLine();
            if (ImGui::Button("Add") && !newKey.empty())
            {
                obj[newKey] = createValueByType(newType);
                newKey.clear();
                mgr_.MarkDirty();
            }

            ImGui::Separator();

            // ---- 既存アイテムの描画 ----
            for (auto it = obj.begin(); it != obj.end(); )
            {
                ImGui::PushID(it.key().c_str());

                // TreeNode（クリック範囲を最小化する）
                bool open = ImGui::TreeNodeEx("##node", ImGuiTreeNodeFlags_NoTreePushOnOpen);

                ImGui::SameLine();
                ImGui::TextUnformatted(it.key().c_str());

                ImGui::SameLine(ImGui::GetWindowWidth() - 40);

                bool wantDelete = ImGui::SmallButton("x");

                if (open)
                {
                    ImGui::TreePush("##inner");
                    if (!wantDelete)
                        drawNode(it.key().c_str(), it.value()); // 値の描画を再帰的に呼び出し
                    ImGui::TreePop();
                }

                if (wantDelete)
                {
                    it = obj.erase(it); // 要素を削除し、イテレータを進める
                    ImGui::PopID();
                    continue;
                }

                ImGui::PopID();
                ++it;
            }

            ImGui::TreePop();
        }


        // =========================================================
        // array
        // =========================================================

        /// <summary>
        /// JSON 配列ノードを ImGui に描画し、要素の編集、要素の追加、配列のクリアを可能にする。
        /// </summary>
        void drawArray(const char* label, json& arr)
        {
            if (!ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
                return;

            // ---- 要素の追加・クリアインターフェース ----
            if (ImGui::SmallButton("+ push null"))
            {
                arr.push_back(nullptr);
                mgr_.MarkDirty();
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("clear"))
            {
                arr.clear();
                mgr_.MarkDirty();
            }

            // ---- 既存アイテムの描画 ----
            for (size_t i = 0; i < arr.size(); )
            {
                ImGui::PushID((int)i);

                std::string idx = "[" + std::to_string(i) + "]";
                bool open = ImGui::TreeNodeEx("##node",
                    ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth);

                ImGui::SameLine();
                ImGui::Text("%s", idx.c_str());

                ImGui::SameLine(ImGui::GetWindowWidth() - 40);
                bool wantDelete = ImGui::SmallButton("x");

                if (open)
                {
                    ImGui::TreePush("##inner");
                    if (!wantDelete)
                        drawNode(idx.c_str(), arr[i]); // 値の描画を再帰的に呼び出し
                    ImGui::TreePop();
                }

                if (wantDelete)
                {
                    arr.erase(arr.begin() + i); // 要素を削除し、イテレータを進める
                    ImGui::PopID();
                    continue;
                }

                ImGui::PopID();
                ++i;
            }

            ImGui::TreePop();
        }

        // =========================================================
        // primitives
        // =========================================================

        /// <summary>
        /// 文字列ノードを ImGui::InputTextDynamic として描画する。
        /// </summary>
        void drawString(const char* label, json& node)
        {
            std::string s = node.get<std::string>();
            if (InputTextDynamic(label, s))
            {
                node = s;
                mgr_.MarkDirty();
            }
        }

        /// <summary>
        /// 整数ノードを ImGui::DragInt として描画する。
        /// </summary>
        void drawInt(const char* label, json& node)
        {
            int v = node.get<int>();
            if (ImGui::DragInt(label, &v, 1.0f))
            {
                node = v;
                mgr_.MarkDirty();
            }
        }

        /// <summary>
        /// 浮動小数点ノードを ImGui::DragFloat として描画する（floatのみ使用）。
        /// </summary>
        void drawFloat(const char* label, json& node)
        {
            float v = node.get<float>();
            if (ImGui::DragFloat(label, &v, 0.1f))
            {
                node = v;
                mgr_.MarkDirty();
            }
        }

        /// <summary>
        /// 真偽値ノードを ImGui::Checkbox として描画する。
        /// </summary>
        void drawBool(const char* label, json& node)
        {
            bool b = node.get<bool>();
            if (ImGui::Checkbox(label, &b))
            {
                node = b;
                mgr_.MarkDirty();
            }
        }

        /// <summary>
        /// Null ノードをプレーンテキストとして描画する。
        /// </summary>
        void drawNull()
        {
            ImGui::Text("null");
        }

    private:
        JsonManager& mgr_; ///< 関連付けられた JsonManager インスタンスへの参照。
    };
#endif // JSONMGR_WITH_IMGUI
} // namespace Utility
} // namespace TYEngine



// ======================
// Vector2
// ======================
/// <summary>
/// nlohmann::json のシリアライゼーション。Vector2 を JSON オブジェクトに変換する。
/// </summary>
inline void to_json(nlohmann::json& j, const Vector2& v)
{
    j = nlohmann::json{
        {"x", v.x},
        {"y", v.y}
    };
}
/// <summary>
/// nlohmann::json のデシリアライゼーション。JSON オブジェクトを Vector2 に変換する。
/// </summary>
inline void from_json(const nlohmann::json& j, Vector2& v)
{
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
}

// ======================
// Vector3
// ======================
/// <summary>
/// nlohmann::json のシリアライゼーション。Vector3 を JSON オブジェクトに変換する。
/// </summary>
inline void to_json(nlohmann::json& j, const Vector3& v)
{
    j = nlohmann::json{
        {"x", v.x},
        {"y", v.y},
        {"z", v.z}
    };
}
/// <summary>
/// nlohmann::json のデシリアライゼーション。JSON オブジェクトを Vector3 に変換する。
/// </summary>
inline void from_json(const nlohmann::json& j, Vector3& v)
{
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
}

// ======================
// Vector4
// ======================
/// <summary>
/// nlohmann::json のシリアライゼーション。Vector4 を JSON オブジェクトに変換する。
/// </summary>
inline void to_json(nlohmann::json& j, const Vector4& v)
{
    j = nlohmann::json{
        {"x", v.x},
        {"y", v.y},
        {"z", v.z},
        {"w", v.w}
    };
}
/// <summary>
/// nlohmann::json のデシリアライゼーション。JSON オブジェクトを Vector4 に変換する。
/// </summary>
inline void from_json(const nlohmann::json& j, Vector4& v)
{
    v.x = j.at("x").get<float>();
    v.y = j.at("y").get<float>();
    v.z = j.at("z").get<float>();
    v.w = j.at("w").get<float>();
}