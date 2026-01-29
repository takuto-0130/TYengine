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
		BYTE* pBuffer; // バッファの先頭アドレス
		unsigned int bufferSize; // バッファのサイズ
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
	// 初期化
	void Initialize(const std::string& directoryPath = "Resources/Sound/");
	
	void Start();

	/**
	 * @brief 音源の停止
	 * @param resourceNum BGMのリソース番号
	 */
	void StopBGM(int resourceNum);


	void Update();                  // ★ 無音バッファ供給
	void EnableSilentFeed(bool enable);

	/**
	 * @brief 音源のポーズ
	 * @param resourceNum サウンドのリソース番号
	 */
	void Pause(int resourceNum);

	/**
	 * @brief 音源の再開
	 * @param resourceNum サウンドのリソース番号
	 */
	void ReStart(int resourceNum);

	/**
	 * @brief 全体音量調整
	 */
	void SetMasterVolume(float volume);

	/**
	 * @brief カテゴリー別音量調整
	 * @param soundCategory サウンドのカテゴリー
	 */
	void SetCategoryVolume(const std::string& soundCategory, float volume);

	/**
	 * @brief 個別音量調整
	 * @param resourceNum サウンドのリソース番号
	 */
	void SetSoundVolume(int resourceNum, float volume);

	/**
	 * @brief 全体音量
	 */
	float GetMasterVolume();

	/**
	 * @brief カテゴリー別音量
	 * @param soundCategory サウンドのカテゴリー
	 */
	float GetCategoryVolume(const std::string& soundCategory);

	/**
	 * @brief 個別音量
	 * @param resourceNum サウンドのリソース番号
	 */
	float GetSoundVolume(int resourceNum);

	// 音声読み込み
	void LoadWave(const std::string& filename);

	// 音声データ解放
	void SoundUnload(const std::string& filename);

	// サウンドカテゴリー追加
	void AddSoundCategory(const std::string& soundCategory);

	/**
	 * @brief 音源の再生
	 * @param soundData 音源データ
	 * @param isLoop ループするか　default : false
	 * @param soundCategory サウンドのカテゴリー
	 * @return int BGMのリソース番号
	 */
	int Play(const std::string& filename, const bool isLoop = false, std::string soundCategory = "");

	MyAnalyzerXAPO* GetAnalyzerXAPO() { return analyzerXAPO_; }

	int GetAnalyzerSampleRate()
	{ 
		XAUDIO2_VOICE_DETAILS submixDetails = {};
		analyzerSubmix_->GetVoiceDetails(&submixDetails);

		return submixDetails.InputSampleRate;
	}


private:
	// 利用可能なソースボイスを検索
	int SearchSourceVoice(IXAudio2SourceVoice** sourceVoices);

	XAUDIO2_BUFFER SetBuffer(bool loop, const SoundData& sound);

	void CreateAnalyzerSubmix();

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	std::unordered_map<std::string, IXAudio2SubmixVoice*> soundCategorySubmixVoices_;

	// サウンドデータ格納コンテナ
	std::unordered_map<std::string, Audio::SoundData> soundDataMap_;

	// 再生中データコンテナ
	std::array<IXAudio2SourceVoice*, kMaxPlayWave> sourceVoices_ = { nullptr };




	// サウンド格納ディレクトリ
	std::string directoryPath_;

	// XAPO のインスタンス
	MyAnalyzerXAPO* analyzerXAPO_ = nullptr;

	// 全体解析用 Submix
	IXAudio2SubmixVoice* analyzerSubmix_ = nullptr;

	// ---- 無音バッファ用 Voice ----
	IXAudio2SourceVoice* silentVoice_ = nullptr;
	WAVEFORMATEX silentFormat_{};
	std::vector<float> silentBuffer_;
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
