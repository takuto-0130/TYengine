#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <fstream>
#include <wrl.h>
#include <mutex>
#include <unordered_map>
#include <array>
#include <vector>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <Logger.h>
#include <xaudio2fx.h>

#include "MyAnalyzerXAPO.h"
#include "StreamingAudio.h"
#include "SingletonObject.h"


// WAVヘッダーの定義
struct WAVHeader {
	char riff[4];			// "RIFF"
	uint32_t size;			// ファイルサイズ
	char wave[4];			// "WAVE"
	char fmt[4];			// "fmt "
	uint32_t fmtSize;		// fmtチャンクのサイズ
	uint16_t audioFormat;	// オーディオフォーマット（1はPCM）
	uint16_t numChannels;	// チャネル数
	uint32_t sampleRate;	// サンプルレート
	uint32_t byteRate;		// バイトレート
	uint16_t blockAlign;	// ブロックアライメント
	uint16_t bitsPerSample;	// サンプルあたりのビット数
	char data[4];			// "data"
	uint32_t dataSize;		// データチャンクのサイズ
};

// 再生用コールバック
class StreamingVoiceCallback : public IXAudio2VoiceCallback {
public:
	void STDMETHODCALLTYPE OnBufferEnd([[maybe_unused]]void* pBufferContext) override {
		std::lock_guard<std::mutex> lock(mutex);
		bufferAvailable = true;
		cv.notify_one();
	}

	void STDMETHODCALLTYPE OnBufferStart(void*) override {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
	void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
	void STDMETHODCALLTYPE OnStreamEnd() override {}
	void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
	void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}

	void WaitForBuffer() {
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this] { return bufferAvailable; });
		bufferAvailable = false;
	}

private:
	std::mutex mutex;
	std::condition_variable cv;
	bool bufferAvailable = false;
};

// 音源の同時再生数
static const size_t kMaxPlayWave = 100;

/// <summary>
/// オーディオ再生・管理を行うシングルトンクラス。
/// XAudio2 を使用してWAVファイルの読み込み、再生、ストリーミング、エフェクト適用を行う。
/// </summary>
class Audio :
	public SingletonObject<Audio>
{
	friend class SingletonObject<Audio>;
	friend struct std::default_delete<Audio>;

private:
	Audio()
		: masterVoice_(nullptr)
		, isStreaming_(false)
		, isLoopStreaming_(false)
		, streamVoice_(nullptr)
		, BUFFER_SIZE(0)
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
	};

#pragma region
public:
	void StartStreaming(const char* filename, bool isLoop = false) {
		// すでにストリーミング中の場合は終了
		if (isStreaming_.load()) {
			StopStreaming();
		}

		isLoopStreaming_.store(isLoop);

		isStreaming_.store(true);

		// デタッチ可能なスレッドでストリーミングを開始
		audioThread_ = std::make_unique<std::thread>(&Audio::StreamAudio, this, filename);
	}

	void StopStreaming() {
        if (isStreaming_.load()) {
            isStreaming_.store(false); // ストリーミングを停止するフラグをセット

            if (audioThread_ && audioThread_->joinable()) {
                audioThread_->join(); // スレッドが終了するのを待つ
				Logger::Log("Stop streaming thread.\n");
            }
        }
    }

	void SetPitch(float pitch) {
		if(streamVoice_)
		{
			streamVoice_->SetFrequencyRatio(pitch);
		}
	}

	/**
	 * @brief エフェクトチェーンの設定
	 * @param effectsNum 設定したいエフェクトの数 
	 * @param effects エフェクトの設定の配列
	 */
	void SetEffectChain(uint32_t effectsNum, XAUDIO2_EFFECT_DESCRIPTOR* effects) {
		effectChain_.EffectCount = effectsNum;
		effectChain_.pEffectDescriptors = effects;
	}

	/**
	 * @brief エフェクトチェーンの適用
	 */
	void ApplyEffectChain() {
		if (streamVoice_) {
			// エフェクトチェーンの適用
			if (FAILED(streamVoice_->SetEffectChain(&effectChain_)))
			{
				Logger::Log("Failed to set effect chain.");
			}
		}
		else {
			Logger::Log("Uninitialized streamVoice.");
		}
	}

	/**
	 * @brief エフェクトの設定と有効化（現状一つのみ対応）
	 * @param parameters プリセット、"XAUDIO2FX_I3DL2_PRESET_~" を入れるか自力で設定
	 */
	void SetEffect(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS parameters = {}) {
		if (streamVoice_)
		{
			ReverbConvertI3DL2ToNative(&parameters, &reverbParameters_);
			if (FAILED(streamVoice_->SetEffectParameters(0, &reverbParameters_, sizeof(reverbParameters_)))) {
				Logger::Log("Failed to set effect parameters.");
			}
			streamVoice_->EnableEffect(0);
		}
	}

	void DisableEffect() {
		if (streamVoice_)
		{
			streamVoice_->DisableEffect(0);
		}
	}
private:
	void StreamAudio(const char* filename);

	/**
	 * @brief エフェクトチェーンの初期化
	 */
	void InitEffectChain() {
		// リバーブエフェクトを作成
		IUnknown* reverbEffect = nullptr;
		if (FAILED(XAudio2CreateReverb(&reverbEffect))) // Reverbエフェクトを作成
		{
			Logger::Log("Failed to create reverb effect.");
		}
		else {
			Logger::Log("succeeded to create reverb effect.");
		}
		// エフェクトチェーンの設定
		effect_[0].pEffect = reverbEffect;  // リバーブエフェクトのインターフェース
		effect_[0].InitialState = FALSE;		// 初期状態で無効化
		effect_[0].OutputChannels = 2;      // ステレオ出力
		effectChain_.EffectCount = 1;
		effectChain_.pEffectDescriptors = effect_;
		ApplyEffectChain();
		reverbEffect->Release();
	}
#pragma endregion

public:
	/// <summary>
	/// 初期化処理。
	/// XAudio2エンジンの初期化、マスターボイスの生成を行う。
	/// </summary>
	/// <param name="directoryPath">サウンドファイルのデフォルトディレクトリパス。</param>
	void Initialize(const std::string& directoryPath = "Resources/Sound/");
	
	/// <summary>
	/// オーディオエンジンの開始処理。
	/// </summary>
	void Start();

	/// <summary>
	/// 指定したリソース番号のBGM（音源）を停止する。
	/// </summary>
	/// <param name="resourceNum">停止対象のBGMリソース番号。</param>
	void StopBGM(int resourceNum);


	void Update();                  // ★ 無音バッファ供給
	void EnableSilentFeed(bool enable);

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

	Microsoft::WRL::ComPtr<MyAnalyzerXAPO> GetAnalyzerXAPO() { return analyzerXAPO_; }

	int GetAnalyzerSampleRate()
	{ 
		XAUDIO2_VOICE_DETAILS submixDetails = {};
		analyzerSubmix_->GetVoiceDetails(&submixDetails);

		return submixDetails.InputSampleRate;
	}


private:
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

private:
	/// <summary>XAudio2 エンジン本体。</summary>
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	/// <summary>マスターボイス。</summary>
	IXAudio2MasteringVoice* masterVoice_;

	/// <summary>サウンドカテゴリごとのサブミックスボイス。</summary>
	std::unordered_map<std::string, IXAudio2SubmixVoice*> soundCategorySubmixVoices_;

	/// <summary>ロード済みサウンドデータ格納コンテナ。</summary>
	std::unordered_map<std::string, Audio::SoundData> soundDataMap_;

	/// <summary>再生中のソースボイス配列（同時再生管理用）。</summary>
	std::array<IXAudio2SourceVoice*, kMaxPlayWave> sourceVoices_ = { nullptr };




	/// <summary>サウンドファイルのルートディレクトリパス。</summary>
	std::string directoryPath_;

	/// <summary>波形解析用 XAPO インスタンス。</summary>
	Microsoft::WRL::ComPtr<MyAnalyzerXAPO> analyzerXAPO_;

	/// <summary>全体解析用サブミックスボイス。</summary>
	IXAudio2SubmixVoice* analyzerSubmix_ = nullptr;

	// ---- 無音バッファ用 Voice ----
	/// <summary>無音再生用ソースボイス（オーディオエンジン維持用）。</summary>
	IXAudio2SourceVoice* silentVoice_ = nullptr;
	/// <summary>無音フォーマット。</summary>
	WAVEFORMATEX silentFormat_{};
	/// <summary>無音バッファデータ。</summary>
	std::vector<float> silentBuffer_;
	/// <summary>無音フィードが有効かどうか。</summary>
	bool silentFeedEnabled_ = false;

private: // 削除予定
	// ストリーミング再生
	std::atomic<bool> isStreaming_;
	std::atomic<bool> isLoopStreaming_;
	std::unique_ptr<std::thread> audioThread_;
	IXAudio2SourceVoice* streamVoice_ = nullptr;
	std::vector<std::vector<BYTE>> audioBuffers_;
	size_t BUFFER_SIZE;
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters_ = {};
	XAUDIO2_EFFECT_CHAIN effectChain_ = {};
	XAUDIO2_EFFECT_DESCRIPTOR effect_[1] = {};
};
