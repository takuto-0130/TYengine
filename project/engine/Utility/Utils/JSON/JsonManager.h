#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cstdio>

#include <nlohmann/json.hpp>

namespace jx
{

    using json = nlohmann::json;

    // ============================================================
    // JSON のパス解析
    // ============================================================
    // ・ "Stage[1].Objects[5].pos.x" などの文字列から
    //    key / index を分解して利用するための仕組み
    // ・ JSON Pointer より扱いやすい独自仕様

    struct PathToken
    {
        enum class Kind { Key, Index };
        Kind kind{};
        std::string key;  // Key の場合の文字列
        size_t index{};   // Index の場合の配列番号
    };

    class Path
    {
    public:
        std::vector<PathToken> tokens;

        // パス文字列を解析して PathToken の列へ変換
        static Path Parse(std::string_view text, bool allow_slash_in_key = false)
        {
            Path p; p.tokens.reserve(8);
            std::string cur;

            // 現在のバッファに溜めたキー文字列を tokens に登録
            auto flush_key = [&]()
                {
                    if (!cur.empty())
                    {
                        p.tokens.push_back({ PathToken::Kind::Key, cur, 0 });
                        cur.clear();
                    }
                };

            for (size_t i = 0; i < text.size(); ++i)
            {
                char c = text[i];

                // ドットでキー区切り
                if (c == '.')
                {
                    flush_key();
                }
                // 配列インデックス [number]
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
                        throw std::runtime_error("パス中のインデックスが不正です: " + std::string(text));

                    p.tokens.push_back({ PathToken::Kind::Index, {}, val });
                    i = j;
                }
                // / も区切りとして扱う（任意）
                else if (c == '/' && !allow_slash_in_key)
                {
                    flush_key();
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

    // ============================================================
    // JsonManager 本体
    // ============================================================
    class JsonManager
    {
    public:
        // JSON の内容チェック用。false を返すとロードエラー扱い
        using ValidateFn = std::function<bool(json& root, std::string& out_msg)>;

        // 古いバージョンからの移行処理
        using MigrateFn = std::function<void(int from_version, json& root)>;

        struct Options
        {
            int  pretty_indent = 2;            // 保存時のインデント幅
            bool autosave = false;             // 値変更時に自動保存するか
            bool make_backup_on_save = true;   // 保存前に *.bak を作成するか
            std::string meta_key = "_meta";   // バージョン情報などを保持する領域名
            std::string version_key = "schema"; // スキーマバージョン番号キー
        };

    public:
        JsonManager() = default;
        explicit JsonManager(Options opt) : opt_(opt) {}

        // ------------------------------------------------------------
        // JSON の読み込み
        // ------------------------------------------------------------
        bool Load(const std::filesystem::path& file, bool create_if_missing = true, std::string* err = nullptr)
        {
            last_path_ = file;
            std::error_code ec;
            bool exists = std::filesystem::exists(file, ec);

            // ファイルが無い場合
            if (!exists)
            {
                // 生成しない設定ならエラー
                if (!create_if_missing)
                {
                    if (err) *err = "ファイルが存在しません";
                    return false;
                }
                // 空の JSON を作成
                root_ = json::object();
                dirty_ = true;
                return Save(last_path_, err);
            }

            // 存在するなら読み込み
            try
            {
                std::ifstream ifs(file, std::ios::binary);
                if (!ifs)
                {
                    if (err) *err = "ファイルを開けません";
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

        // ------------------------------------------------------------
        // JSON の保存
        // ------------------------------------------------------------
        bool Save(std::optional<std::filesystem::path> file = std::nullopt, std::string* err = nullptr)
        {
            const auto& path = file ? *file : last_path_;
            if (path.empty())
            {
                if (err) *err = "保存先パスが設定されていません";
                return false;
            }

            try
            {
                // バックアップ作成
                if (opt_.make_backup_on_save && std::filesystem::exists(path))
                {
                    std::filesystem::path bak = path;
                    bak += ".bak";
                    std::error_code ec;
                    std::filesystem::copy_file(path, bak, std::filesystem::copy_options::overwrite_existing, ec);
                }

                std::ofstream ofs(path, std::ios::binary | std::ios::trunc);
                if (!ofs)
                {
                    if (err) *err = "書き込み用にファイルを開けません";
                    return false;
                }

                if (opt_.pretty_indent > 0)
                    ofs << root_.dump(opt_.pretty_indent);
                else
                    ofs << root_.dump();

                ofs.flush();
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

        // ============================================================
        // バージョン管理
        // ============================================================
        int GetSchemaVersion() const
        {
            if (!root_.contains(opt_.meta_key)) return 0;
            const json& m = root_.at(opt_.meta_key);
            if (m.is_object() && m.contains(opt_.version_key) && m.at(opt_.version_key).is_number_integer())
                return m.at(opt_.version_key).get<int>();
            return 0;
        }

        void SetSchemaVersion(int v)
        {
            root_[opt_.meta_key][opt_.version_key] = v;
            dirty_ = true;
            maybeAutosave();
        }

        void SetValidator(ValidateFn v) { validator_ = std::move(v); }
        void SetMigrator(MigrateFn m) { migrator_ = std::move(m); }

        // 読み込み後のバリデーション + バージョン移行
        bool ValidateAndMigrate(int target_version = -1, std::string* err = nullptr)
        {
            try
            {
                int cur = GetSchemaVersion();

                // バージョン移行が必要な場合
                if (migrator_ && target_version >= 0 && cur < target_version)
                {
                    migrator_(cur, root_);
                    SetSchemaVersion(target_version);
                }

                // バリデーション
                if (validator_)
                {
                    std::string msg;
                    bool ok = validator_(root_, msg);
                    if (!ok)
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

        // ============================================================
        // パスアクセス
        // ============================================================
        json* Find(std::string_view path) { return findImpl(Path::Parse(path)); }
        const json* Find(std::string_view path) const { return const_cast<JsonManager*>(this)->findImpl(Path::Parse(path)); }

        // 存在しないノードは自動生成して返す
        json& Ensure(std::string_view path) { return ensureImpl(Path::Parse(path)); }
        // 値取得（存在しなければデフォルト）
        template<typename T>
        T Get(std::string_view path, T default_value = {}) const
        {
            const json* n = Find(path);
            if (!n) return default_value;
            try { return n->get<T>(); }
            catch (...) { return default_value; }
        }

        // 値設定（ノードが無ければ自動作成）
        template<typename T>
        bool Set(std::string_view path, const T& value)
        {
            try
            {
                json& n = Ensure(path);
                n = value;
                dirty_ = true;
                maybeAutosave();
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        // ノード削除
        bool Erase(std::string_view path)
        {
            Path p = Path::Parse(path);
            if (p.tokens.empty()) return false;

            json* parent = &root_;
            for (size_t i = 0; i + 1 < p.tokens.size(); ++i)
            {
                auto& t = p.tokens[i];

                // object.key
                if (t.kind == PathToken::Kind::Key)
                {
                    if (!parent->is_object() || !parent->contains(t.key)) return false;
                    parent = &(*parent)[t.key];
                }
                // array[index]
                else
                {
                    if (!parent->is_array() || t.index >= parent->size()) return false;
                    parent = &(*parent)[t.index];
                }
            }

            // 最終ノードの削除
            auto& last = p.tokens.back();
            bool removed = false;

            if (last.kind == PathToken::Kind::Key)
            {
                if (parent->is_object()) removed = parent->erase(last.key) > 0;
            }
            else
            {
                if (parent->is_array() && last.index < parent->size())
                {
                    parent->erase(parent->begin() + static_cast<json::difference_type>(last.index));
                    removed = true;
                }
            }

            if (removed)
            {
                dirty_ = true;
                maybeAutosave();
            }
            return removed;
        }

        // 全体 JSON にアクセス
        json& Root() { return root_; }
        const json& Root() const { return root_; }

        // Dirty 状態（未保存変更があるか）
        bool IsDirty() const { return dirty_; }
        void MarkDirty(bool d = true)
        {
            dirty_ = d;
            if (dirty_) maybeAutosave();
        }

    private:
        // ------------------------------------------------------------
        // findImpl : ノード探索
        // ------------------------------------------------------------
        json* findImpl(const Path& p)
        {
            json* n = &root_;
            for (const auto& t : p.tokens)
            {
                // object.key
                if (t.kind == PathToken::Kind::Key)
                {
                    if (!n->is_object()) return nullptr;
                    auto it = n->find(t.key);
                    if (it == n->end()) return nullptr;
                    n = &(*it);
                }
                // array[index]
                else
                {
                    if (!n->is_array() || t.index >= n->size()) return nullptr;
                    n = &(*n)[t.index];
                }
            }
            return n;
        }

        // ------------------------------------------------------------
        // ensureImpl : 無ければ自動作成しながらパスを辿る
        // ------------------------------------------------------------
        json& ensureImpl(const Path& p)
        {
            json* n = &root_;

            for (const auto& t : p.tokens)
            {
                // object.key
                if (t.kind == PathToken::Kind::Key)
                {
                    if (!n->is_object()) *n = json::object();
                    n = &((*n)[t.key]);
                }
                // array[index]
                else
                {
                    if (!n->is_array()) *n = json::array();
                    auto& arr = *n;

                    // resize() が使えない環境のため push_back 方式で拡張
                    while (arr.size() <= t.index)
                        arr.push_back(json());

                    n = &arr[t.index];
                }
            }
            return *n;
        }

    private:
        // autosave が有効なら保存
        void maybeAutosave()
        {
            if (opt_.autosave && !last_path_.empty())
            {
                std::string err;
                Save(last_path_, &err);
            }
        }

    private:
        json root_ = json::object();       // JSON ドキュメント本体
        bool dirty_ = false;               // 未保存の変更があるか
        std::filesystem::path last_path_;  // 最後の保存/読み込みパス
        Options opt_{};                    // 各種オプション

        ValidateFn validator_{};
        MigrateFn  migrator_{};
    };

    // ============================================================
    // ImGui による JSON 編集 GUI（任意）
    // ============================================================
#ifdef JSONMGR_WITH_IMGUI
#include <imgui.h>

    namespace detail
    {
        // 安全で ImGui 公式推奨の可変長文字列入力
        inline bool InputTextDynamic(const char* label, std::string& str)
        {
            // 固定バッファではなく、ImGui公式のコールバック方式を使う
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_CallbackResize;

            // ImGui が編集するバッファとして std::string を直接渡す
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
    }

    // ------------------------------------------------------------
    // JSON ImGui エディタ
    // ------------------------------------------------------------
    class JsonImGuiEditor
    {
    public:
        explicit JsonImGuiEditor(JsonManager& mgr) : mgr_(mgr) {}

        // JSON 全体または部分ノードを表示
        void Draw(json& node, const char* label = "root")
        {
            ImGui::PushID(label);
            drawNode(label, node);
            ImGui::PopID();
        }

    private:
        // ノードの型に応じて描画を振り分け
        void drawNode(const char* label, json& node)
        {
            using Type = json::value_t;
            Type t = node.type();

            switch (t)
            {
            case Type::object: drawObject(label, node); break;
            case Type::array:  drawArray(label, node); break;
            case Type::string: drawString(label, node); break;
            case Type::number_integer: drawInt(label, node); break;
            case Type::number_unsigned: drawUInt(label, node); break;
            case Type::number_float: drawFloat(label, node); break;
            case Type::boolean: drawBool(label, node); break;
            case Type::null: drawNull(label, node); break;
            default: ImGui::TextUnformatted("TextUnformatted"); break;
            }
        }

        // ------------------------
        // object の描画
        // ------------------------
        void drawObject(const char* label, json& obj)
        {
            if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
            {

                // 新規キー追加 UI
                ImGui::PushID(label);

                // ★ JSON オブジェクトのアドレスを ID に使う（絶対にユニーク）
                ImGuiID id = ImGui::GetID((void*)&obj);
                std::string& newKey = keyInputMap[id];

                ImGui::Separator();
                ImGui::TextUnformatted("AddKey:");
                ImGui::SameLine();
                float fullWidth = ImGui::GetContentRegionAvail().x;

                // ボタンが占有する幅をあらかじめ引く
                float buttonWidth = 6 * 60.0f;   // +empty +str +num +bool +obj +arr の分
                float inputWidth = fullWidth - buttonWidth;

                if (inputWidth < 100.0f) inputWidth = 100.0f; // 最低幅

                ImGui::SetNextItemWidth(inputWidth);
                detail::InputTextDynamic("##AddKey", newKey);

                ImGui::SameLine();
                if (ImGui::Button("+empty") && !newKey.empty())
                {
                    obj[newKey] = nullptr;
                    newKey.clear();
                    mgr_.MarkDirty();
                }
                ImGui::SameLine();
                if (ImGui::Button("+str") && !newKey.empty())
                {
                    obj[newKey] = std::string("");
                    newKey.clear();
                    mgr_.MarkDirty();
                }
                ImGui::SameLine();
                if (ImGui::Button("+num") && !newKey.empty())
                {
                    obj[newKey] = 0.0;
                    newKey.clear();
                    mgr_.MarkDirty();
                }
                ImGui::SameLine();
                if (ImGui::Button("+bool") && !newKey.empty())
                {
                    obj[newKey] = false;
                    newKey.clear();
                    mgr_.MarkDirty();
                }
                ImGui::SameLine();
                if (ImGui::Button("+obj") && !newKey.empty())
                {
                    obj[newKey] = json::object();
                    newKey.clear();
                    mgr_.MarkDirty();
                }
                ImGui::SameLine();
                if (ImGui::Button("+arr") && !newKey.empty())
                {
                    obj[newKey] = json::array();
                    newKey.clear();
                    mgr_.MarkDirty();
                }

                ImGui::Separator();

                // 既存キーの表示・編集
                for (auto it = obj.begin(); it != obj.end(); )
                {
                    ImGui::PushID(it.key().c_str());

                    bool open = ImGui::TreeNodeEx("##icon", ImGuiTreeNodeFlags_NoTreePushOnOpen);

                    ImGui::SameLine();
                    ImGui::Text("%s", it.key().c_str());

                    ImGui::SameLine(ImGui::GetWindowWidth() - 30);
                    bool wantDelete = ImGui::SmallButton("x");

                    if (open)
                    {
                        ImGui::TreePush("##dummy");
                        if (!wantDelete)
                        {
                            drawNode(it.key().c_str(), it.value());
                        }
                        ImGui::TreePop();
                    }

                    if (wantDelete)
                    {
                        it = obj.erase(it);
                        ImGui::PopID();
                        continue;
                    }

                    ImGui::PopID();
                    ++it;
                }

                ImGui::PopID();
                ImGui::TreePop();
            }
        }

        // ------------------------
        // array の描画
        // ------------------------
        void drawArray(const char* label, json& arr)
        {
            if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
            {

                // 要素追加とクリア
                if (ImGui::SmallButton("+ push")) arr.push_back(nullptr);
                ImGui::SameLine(); if (ImGui::SmallButton("clear")) arr.clear();

                // 各要素の表示
                for (size_t i = 0; i < arr.size(); ++i)
                {
                    ImGui::PushID(static_cast<int>(i));
                    std::string ilabel = std::string("[") + std::to_string(i) + "]";

                    bool open = ImGui::TreeNodeEx(ilabel.c_str(), ImGuiTreeNodeFlags_SpanAvailWidth);

                    ImGui::SameLine();
                    if (ImGui::SmallButton("x"))
                    {
                        arr.erase(arr.begin() + static_cast<long long>(i));
                        if (open) ImGui::TreePop();
                        ImGui::PopID();
                        break;
                    }

                    if (open)
                    {
                        drawNode(ilabel.c_str(), arr[i]);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }

        // ------------------------
        // string の描画
        // ------------------------
        void drawString(const char* label, json& node)
        {
            std::string s = node.get<std::string>();
            if (detail::InputTextDynamic(label, s))
            {
                node = s;
                mgr_.MarkDirty();
            }
        }

        // ------------------------
        // integer
        // ------------------------
        void drawInt(const char* label, json& node)
        {
            long long v = node.get<long long>();
            if (ImGui::InputScalar(label, ImGuiDataType_S64, &v))
            {
                node = v;
                mgr_.MarkDirty();
            }
        }

        // ------------------------
        // unsigned integer
        // ------------------------
        void drawUInt(const char* label, json& node)
        {
            unsigned long long v = node.get<unsigned long long>();
            if (ImGui::InputScalar(label, ImGuiDataType_U64, &v))
            {
                node = v;
                mgr_.MarkDirty();
            }
        }

        // ------------------------
        // float / double
        // ------------------------
        void drawFloat(const char* label, json& node)
        {
            double v = node.get<double>();
            if (ImGui::InputDouble(label, &v))
            {
                node = v;
                mgr_.MarkDirty();
            }
        }

        // ------------------------
        // bool
        // ------------------------
        void drawBool(const char* label, json& node)
        {
            bool b = node.get<bool>();
            if (ImGui::Checkbox(label, &b))
            {
                node = b;
                mgr_.MarkDirty();
            }
        }

        // ------------------------
        // null
        // ------------------------
        void drawNull(const char* label, json& node)
        {
            (void)label;
            ImGui::TextUnformatted("null"); ImGui::SameLine();

            if (ImGui::SmallButton("make obj")) { node = json::object(); mgr_.MarkDirty(); }
            ImGui::SameLine(); if (ImGui::SmallButton("make arr")) { node = json::array(); mgr_.MarkDirty(); }
            ImGui::SameLine(); if (ImGui::SmallButton("make str")) { node = std::string(""); mgr_.MarkDirty(); }
            ImGui::SameLine(); if (ImGui::SmallButton("make num")) { node = 0.0; mgr_.MarkDirty(); }
            ImGui::SameLine(); if (ImGui::SmallButton("make bool")) { node = false; mgr_.MarkDirty(); }
        }

    private:
        JsonManager& mgr_;
        std::unordered_map<ImGuiID, std::string> keyInputMap;
    };
#endif // JSONMGR_WITH_IMGUI

    // ------------------------------------------------------------
    // ここから下は、ユーザーのステージ定義に特化した補助関数サンプル
    // （必要なければ使わない or 削除して問題なし）
    // ------------------------------------------------------------

    // struct StageObjectDef {
    //     std::string name;
    //     int id{};
    // };

    // // Stage/ObjectDefs と Stage/Checkpoints の骨組みを確実に用意
    // inline void EnsureStageSchema(JsonManager& jm) {
    //     auto& defs = jm.Ensure("Stage/ObjectDefs");
    //     if (!defs.is_array()) defs = json::array();
    //
    //     auto& cps = jm.Ensure("Stage/Checkpoints");
    //     if (!cps.is_array()) cps = json::array();
    // }

    // // オブジェクト定義を追加
    // inline void AddStageObjectDef(JsonManager& jm, const StageObjectDef& d) {
    //     json j;
    //     j["name"] = d.name;
    //     j["id"]   = d.id;
    //     jm.Ensure("Stage/ObjectDefs").push_back(std::move(j));
    //     jm.MarkDirty();
    // }

    // // チェックポイントを追加
    // inline void AddCheckpoint(JsonManager& jm, float height) {
    //     json cp;
    //     cp["height"] = height;
    //     cp["objects"] = json::array();
    //
    //     jm.Ensure("Stage/Checkpoints").push_back(std::move(cp));
    //     jm.MarkDirty();
    // }

    // // チェックポイント内にオブジェクト配置を追加
    // inline void AddObjectToCheckpoint(JsonManager& jm, size_t cpIndex,
    //                                  int defId, float x, float y, float z) {
    //     auto* cps = jm.Find("Stage/Checkpoints");
    //     if (!cps || !cps->is_array() || cpIndex >= cps->size()) return;
    //
    //     json obj;
    //     obj["defId"] = defId;
    //     obj["pos"]   = { x, y, z };
    //
    //     (*cps)[cpIndex]["objects"].push_back(std::move(obj));
    //     jm.MarkDirty();
    // }

} // namespace jx

