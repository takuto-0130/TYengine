#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <fstream>
#include <wrl.h>
#include <unordered_map>
#include <array>
#include <vector>
#include <xaudio2fx.h>

#include "MyAnalyzerXAPO.h"
#include "StreamingAudio.h"
#include "SingletonObject.h"

namespace TYEngine
{
	namespace AudioSystem
	{

		// 音源の同時再生数
		static const size_t kMaxPlayWave = 100;

		/// <summary>
		/// オーディオ再生・管理を行うシングルトンクラス。
		/// XAudio2 を使用してWAVファイルの読み込み、再生を行う。
		/// </summary>
		class Audio :
			public Utility::SingletonObject<Audio>
		{
			friend class Utility::SingletonObject<Audio>;
			friend struct std::default_delete<Audio>;

		private:
			Audio()
				: masterVoice_(nullptr)
			{}
			~Audio();

		private: // 構造体
			// submixのプロセスステージ（上から順に下層）
			enum mixStage
			{
				kCategory,
				kAnalyzer
			};

			// チャンクヘッダー
			struct ChunkHeader
			{
				char id[4]; // チャンクごとのID
				int32_t size; // チャンクサイズ
			};

			// RIFFヘッダチャンク
			struct RiffHeader
			{
				ChunkHeader chunk; // "RIFF"
				char type[4]; // "WAVE"
			};

			// FMTチャンク
			struct FormatChunk
			{
				ChunkHeader chunk; // "fmt"
				WAVEFORMATEX fmt; // 波形フォーマット
			};

			// 音声データ
			struct SoundData
			{
				WAVEFORMATEX wfex; // 波形フォーマット
				std::vector<BYTE> buffer; // バッファの先頭アドレス
				int playSoundLength;
				float bpm; // BPMデータ
			};

		public:
			/// <summary>
			/// 初期化処理。
			/// XAudio2エンジンの初期化、マスターボイスの生成を行う。
			/// </summary>
			/// <param name="directoryPath">サウンドファイルのデフォルトディレクトリパス。</param>
			void Initialize(const std::string& directoryPath = "Resources/Sound/");

			/// <summary>
			///  無音バッファ供給。
			/// </summary>
			void Update();

			/// <summary>
			///  無音バッファフラグ。
			/// </summary>
			void EnableSilentFeed(bool enable);

			/// <summary>
			/// 指定したリソース番号のBGM（音源）を停止する。
			/// </summary>
			/// <param name="resourceNum">停止対象のBGMリソース番号。</param>
			void StopBGM(int resourceNum);

			/// <summary>
			/// 音源を一時停止する。
			/// </summary>
			/// <param name="resourceNum">対象のサウンドリソース番号。</param>
			void Pause(int resourceNum);

			/// <summary>
			/// 一時停止中の音源を再開する。
			/// </summary>
			/// <param name="resourceNum">対象のサウンドリソース番号。</param>
			void ReStart(int resourceNum);

			/// <summary>
			/// マスターボリューム（全体の音量）を設定する。
			/// </summary>
			/// <param name="volume">音量（0.0f ~ 1.0f）。</param>
			void SetMasterVolume(float volume);

			/// <summary>
			/// 指定カテゴリーの音量を設定する（BGM, SE など）。
			/// </summary>
			/// <param name="soundCategory">カテゴリー名。</param>
			/// <param name="volume">音量。</param>
			void SetCategoryVolume(const std::string& soundCategory, float volume);

			/// <summary>
			/// 個別の音源の音量を設定する。
			/// </summary>
			/// <param name="resourceNum">リソース番号。</param>
			/// <param name="volume">音量。</param>
			void SetSoundVolume(int resourceNum, float volume);

			/// <summary>
			/// 現在のマスターボリュームを取得する。
			/// </summary>
			/// <returns>現在の音量。</returns>
			float GetMasterVolume();

			/// <summary>
			/// カテゴリーごとの音量を取得する。
			/// </summary>
			/// <param name="soundCategory">カテゴリー名。</param>
			/// <returns>音量。</returns>
			float GetCategoryVolume(const std::string& soundCategory);

			/// <summary>
			/// 個別音源の音量を取得する。
			/// </summary>
			/// <param name="resourceNum">リソース番号。</param>
			/// <returns>音量。</returns>
			float GetSoundVolume(int resourceNum);

			/// <summary>
			/// WAVファイルを読み込み、メモリにキャッシュする。
			/// </summary>
			/// <param name="filename">ファイル名（パス含む）。</param>
			void LoadWave(const std::string& filename);

			/// <summary>
			/// 指定したファイルの音声データをメモリから解放する。
			/// </summary>
			/// <param name="filename">解放するファイル名。</param>
			void SoundUnload(const std::string& filename);

			/// <summary>
			/// 新しいサウンドカテゴリーを追加する（BGM, SEなど）。
			/// </summary>
			/// <param name="soundCategory">カテゴリー名。</param>
			void AddSoundCategory(const std::string& soundCategory);

			/// <summary>
			/// 音源を再生する。
			/// </summary>
			/// <param name="filename">再生するファイル名。</param>
			/// <param name="isLoop">ループ再生を行うか（デフォルト: false）。</param>
			/// <param name="soundCategory">再生するカテゴリー（省略時は無し）。</param>
			/// <returns>再生ハンドル（リソース番号）。エラー時は -1。</returns>
			int Play(const std::string& filename, const bool isLoop = false, std::string soundCategory = "");

			Microsoft::WRL::ComPtr<MyAnalyzerXAPO> GetAnalyzerXAPO(const std::string& soundCategory = "");

			int GetAnalyzerSampleRate()
			{
				XAUDIO2_VOICE_DETAILS submixDetails = {};
				analyzerSubmix_->GetVoiceDetails(&submixDetails);
				return submixDetails.InputSampleRate;
			}

			SoundData& GetSoundData(const std::string& filename)
			{
				auto it = soundDataMap_.find(filename);

				if (it == soundDataMap_.end())
				{
					Debugger::Log("Sound not loaded: " + filename + "\n");
					static SoundData emptyData{};
					return emptyData;
				}

				return it->second;
			}

		private:
			/// <summary>
			/// 利用可能な（再生中でない）ソースボイスを検索する。
			/// </summary>
			/// <param name="sourceVoices">検索対象のソースボイス配列。</param>
			/// <returns>利用可能なインデックス。見つからない場合は -1。</returns>
			int SearchSourceVoice(IXAudio2SourceVoice** sourceVoices);

			/// <summary>
			/// XAudio2用のバッファ構造体を設定する。
			/// </summary>
			/// <param name="loop">ループ再生するか。</param>
			/// <param name="sound">設定するサウンドデータ。</param>
			/// <returns>設定済み XAUDIO2_BUFFER。</returns>
			XAUDIO2_BUFFER SetBuffer(bool loop, const SoundData& sound);

			/// <summary>
			/// 波形解析用のサブミックスボイスを作成・接続する。
			/// </summary>
			void CreateAnalyzerSubmix();

			/// <summary>
			/// オーディオエンジンの開始処理。
			/// </summary>
			void Start();

			/// <summary>
			/// 無音ボイスの作成。
			/// </summary>
			void CreateSilentVoice(IXAudio2SourceVoice*& voice, IXAudio2SubmixVoice* sendSubmix);

		private:
			/// <summary>XAudio2 エンジン本体。</summary>
			Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
			/// <summary>マスターボイス。</summary>
			IXAudio2MasteringVoice* masterVoice_;

			/// <summary>サウンドカテゴリごとのサブミックスボイス。</summary>
			std::unordered_map<std::string, IXAudio2SubmixVoice*> categorySubmixVoices_;
			std::unordered_map<std::string, Microsoft::WRL::ComPtr<MyAnalyzerXAPO>> categoryAnalyzerXAPO_;

			/// <summary>ロード済みサウンドデータ格納コンテナ。</summary>
			std::unordered_map<std::string, SoundData> soundDataMap_;

			/// <summary>再生中のソースボイス配列（同時再生管理用）。</summary>
			std::array<IXAudio2SourceVoice*, kMaxPlayWave> sourceVoices_ = { nullptr };

			/// <summary>波形解析用 XAPO インスタンス。</summary>
			Microsoft::WRL::ComPtr<MyAnalyzerXAPO> analyzerXAPO_;

			/// <summary>全体解析用サブミックスボイス。</summary>
			IXAudio2SubmixVoice* analyzerSubmix_ = nullptr;
			XAUDIO2_VOICE_DETAILS analyzerDetails_ = {};

			// ---- 無音バッファ用 Voice ----
			/// <summary>無音再生用ソースボイス（オーディオエンジン維持用）。</summary>
			IXAudio2SourceVoice* silentVoice_ = nullptr;
			/// <summary>無音フォーマット。</summary>
			WAVEFORMATEX silentFormat_{};
			/// <summary>無音バッファデータ。</summary>
			std::vector<float> silentBuffer_;
			/// <summary>無音フィードが有効かどうか。</summary>
			bool silentFeedEnabled_ = false;

			/// <summary>無音再生用ソースボイス。</summary>
			std::unordered_map<std::string, IXAudio2SourceVoice*> categorySilentVoices_;


			/// <summary>サウンドファイルのルートディレクトリパス。</summary>
			std::string directoryPath_;
		};

	} // namespace Audio
} // namespace TYEngine
