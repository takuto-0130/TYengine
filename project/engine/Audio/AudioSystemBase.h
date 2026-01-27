#pragma once
#include "Audio.h"
#include <string>
#include <unordered_map>
#include <cassert>

// Template引数でユーザー定義のEnumを受け取る
template<typename CategoryEnum>
class AudioSystemBase
{
protected:
	AudioSystemBase()
	{
		audio_ = Audio::GetInstance();
	}

	// Audioクラスへの生アクセスが必要な場合
	Audio* GetAudio() { return audio_; }

public:
	virtual ~AudioSystemBase() = default;

	/**
	 * @brief カテゴリーの登録（初期化時に呼ぶ）
	 * @param type アプリ側で使うEnum
	 * @param name Audioクラスに登録する文字列
	 * @param defaultVolume 初期音量
	 */
	void CreateCategory(CategoryEnum type, const std::string& name, float defaultVolume = 1.0f)
	{
		// マップに登録
		categoryMap_[type] = name;

		// Audioエンジン本体にカテゴリーを作成
		audio_->AddSoundCategory(name);
		audio_->SetCategoryVolume(name, defaultVolume);
	}

	/**
	 * @brief Enum指定で再生
	 */
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

	/**
	 * @brief リソース番号指定で音量設定
	 */
	void SetSoundVolume(int resourceNum, float volume)
	{
		audio_->SetSoundVolume(resourceNum, volume);
	}

	/**
	 * @brief Enum指定で音量設定
	 */
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


	/**
	 * @brief リソース番号指定で音量設定
	 */
	float GetSoundVolume(int resourceNum)
	{
		return audio_->GetSoundVolume(resourceNum);
	}

	/**
	 * @brief Enum指定で音量設定
	 */
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
	Audio* audio_ = nullptr;

	std::unordered_map<CategoryEnum, std::string> categoryMap_;
};

