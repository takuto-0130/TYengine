#pragma once
#include "Audio.h"
#include <string>
#include <unordered_map>
#include <cassert>

namespace TYEngine
{
	namespace AudioSystem
	{

		/// <summary>
		/// アプリケーション固有のサウンドカテゴリ（BGM, SEなど）を管理するための基底クラス。
		/// ユーザー定義のEnumをテンプレート引数にとり、Enumと文字列キーの変換を行う。
		/// </summary>
		template<typename CategoryEnum>
		class AudioSystemBase
		{
		protected:
			AudioSystemBase() : audio_(Audio::GetInstance()) {}


			// Audioクラスへの生アクセスが必要な場合
			/// <summary>Audioサブシステムへのアクセスを取得する。</summary>
			Audio* GetAudio() { return audio_; }

		private:
			// 子クラスで必ず実装（カテゴリ登録など）
			/// <summary>
			/// 初期化時のコールバック。
			/// 継承クラスでカテゴリ登録や初期ロードを実装する。
			/// </summary>
			virtual void OnInit() = 0;

		public:
			virtual ~AudioSystemBase() = default;

			void Init()
			{
				// 子クラス固有の登録処理などを呼ぶ
				OnInit();

				// 初期化済みフラグ
				isInit_ = true;
			}

			void LoadSound(const std::string& filename)
			{
				audio_->LoadWave(filename);
			}

			/// <summary>
			/// カテゴリーを登録する（初期化時に呼ぶ）。
			/// </summary>
			/// <param name="type">アプリ側で定義したEnum。</param>
			/// <param name="name">Audio内部で識別する文字列（キー）。</param>
			/// <param name="defaultVolume">初期音量。</param>
			void CreateCategory(CategoryEnum type, const std::string& name, float defaultVolume = 1.0f)
			{
				// マップに登録
				categoryMap_[type] = name;

				// Audioエンジン本体にカテゴリーを作成
				audio_->AddSoundCategory(name);
				audio_->SetCategoryVolume(name, defaultVolume);
			}

			/// <summary>
			/// Enumを指定してサウンドを再生する。
			/// </summary>
			/// <param name="filename">再生するファイルパス。</param>
			/// <param name="isLoop">ループするかどうか。</param>
			/// <param name="category">所属するカテゴリEnum。</param>
			/// <returns>再生ハンドル（ボイスID）。失敗時はエラーコード。</returns>
			int Play(const std::string& filename, bool isLoop, CategoryEnum category)
			{
				// 登録されていないカテゴリーならエラー（またはデフォルト扱い）
				if (categoryMap_.find(category) == categoryMap_.end())
				{
					// 登録忘れを防ぐため assert を入れるか、デフォルト文字列で再生
					assert(false && "Category not registered!");
					return audio_->Play(filename, isLoop);
				}

				// Enum -> string に変換して再生
				return audio_->Play(filename, isLoop, categoryMap_[category]);
			}

			/// <summary>
			/// リソース番号（ハンドル）を指定して音量を設定する。
			/// </summary>
			void SetSoundVolume(int resourceNum, float volume)
			{
				audio_->SetSoundVolume(resourceNum, volume);
			}

			/// <summary>
			/// カテゴリ単位で音量を一括設定する。
			/// </summary>
			void SetCategoryVolume(CategoryEnum category, float volume)
			{
				if (categoryMap_.find(category) != categoryMap_.end())
				{
					audio_->SetCategoryVolume(categoryMap_[category], volume);
				}
			}

			// 全体音量（Master）
			void SetMasterVolume(float volume)
			{
				audio_->SetMasterVolume(volume);
			}


			/// <summary>
			/// リソース番号（ハンドル）を指定して現在の音量を取得する。
			/// </summary>
			float GetSoundVolume(int resourceNum)
			{
				return audio_->GetSoundVolume(resourceNum);
			}

			/// <summary>
			/// カテゴリの現在の音量設定を取得する。
			/// </summary>
			float GetCategoryVolume(CategoryEnum category)
			{
				if (categoryMap_.find(category) != categoryMap_.end())
				{
					return audio_->GetCategoryVolume(categoryMap_[category]);
				}
				return -1.0f;
			}

			// 全体音量（Master）
			float GetMasterVolume()
			{
				return audio_->GetMasterVolume();
			}

		private:
			/// <summary>オーディオ管理クラスへの参照。</summary>
			Audio* audio_ = nullptr;
			/// <summary>カテゴリEnumと文字列キーの対応マップ。</summary>
			std::unordered_map<CategoryEnum, std::string> categoryMap_;

			/// <summary>初期化済みかどうか。</summary>
			bool isInit_ = false;
		};

	} // namespace Audio
} // namespace TYEngine

