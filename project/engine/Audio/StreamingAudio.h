#pragma once
#include <xaudio2.h>
#include <string>
#include <fstream>
#include <mutex>
#include <array>
#include <vector>
#include <thread>
#include <atomic>
#include <Logger.h>
#include <xaudio2fx.h>

class Audio;

/// <summary>
/// ストリーミング再生を管理するクラス。
/// 長時間のBGMなどを別スレッドでバッファリングしながら再生する。
/// </summary>
class StreamingAudio
{
private:
	// AudioのみがStreamingAudioにアクセスできるようにする
	friend class Audio;

	StreamingAudio(IXAudio2* xAudio2, const std::string& directoryPath) : xAudio2_(xAudio2), directoryPath_(directoryPath), id_(nextID_++) 
	{}

	~StreamingAudio();

private:
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
	class StreamingVoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		// コンストラクタで初期空きバッファ数を設定（3つ）
		StreamingVoiceCallback() : freeBufferCount_(0) {}

		void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override
		{
			(void)pBufferContext;
			std::lock_guard<std::mutex> lock(mutex_);
			freeBufferCount_++; // 再生が終わったので空きを増やす
			cv_.notify_one();
		}

		// 最初に空きをするためのメソッド
		void Initialize(int count)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			freeBufferCount_ = count;
		}

		// 空きが出るまで待つ
		void WaitForBuffer()
		{
			std::unique_lock<std::mutex> lock(mutex_);
			cv_.wait(lock, [this] { return freeBufferCount_ > 0; }); // 空きバッファができるまで待機
			freeBufferCount_--; // 使うので空きを減らす
		}

		// 空きを減らす
		void ConsumeOneBuffer()
		{
			freeBufferCount_--;
		}

		// 他のメソッドは空実装
		void STDMETHODCALLTYPE OnBufferStart(void*) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
		void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}

	private:
		std::mutex mutex_;
		std::condition_variable cv_;
		int freeBufferCount_; // 空いているバッファの数
	};

public:
	void StartStreaming(const char* filename, bool isLoop = false);

	void StopStreaming();

	void SetPitch(float pitch);

	/// <summary>
	/// エフェクトチェーンの設定を行う。
	/// </summary>
	/// <param name="effectsNum">設定したいエフェクトの数。</param>
	/// <param name="effects">エフェクト設定配列へのポインタ。</param>
	void SetEffectChain(uint32_t effectsNum, XAUDIO2_EFFECT_DESCRIPTOR* effects) {
		effectChain_.EffectCount = effectsNum;
		effectChain_.pEffectDescriptors = effects;
	}

	/// <summary>
	/// 設定されたエフェクトチェーンをボイスに適用する。
	/// </summary>
	void ApplyEffectChain();

	/// <summary>
	/// リバーブエフェクトの設定と有効化を行う（現状1つのみ対応）。
	/// </summary>
	/// <param name="parameters">リバーブパラメータ（プリセットを使用可能）。</param>
	void SetEffect(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS parameters = {});

	/// <summary>
	/// エフェクトを無効化する。
	/// </summary>
	void DisableEffect();

private:
	/// <summary>
	/// 実際のストリーミング処理。
	/// </summary>
	void StreamAudio(const char* filename);

	void PlayStream(std::ifstream& file);

	/**
	 * @brief エフェクトチェーンの初期化
	 */
	void InitEffectChain();



	// ファイルからデータを読み込む関数
	bool ReadAudioData(std::ifstream& file, std::vector<BYTE>& buffer);

	// WAVファイルのヘッダーを読み込む
	bool ReadWavHeader(std::string filename, WAVHeader& header);

public:
	uint32_t GetId() { return id_; }

private:
	IXAudio2* xAudio2_;

	// サウンド格納ディレクトリ
	std::string directoryPath_;

	// ID
	static uint32_t nextID_;
	uint32_t id_;


	// ストリーミング再生
	/// <summary>ストリーミング再生中フラグ。</summary>
	std::atomic<bool> isStreaming_;
	/// <summary>ループ再生フラグ。</summary>
	std::atomic<bool> isLoopStreaming_;
	/// <summary>ストリーミング用スレッド。</summary>
	std::unique_ptr<std::thread> audioThread_;
	/// <summary>ストリーミング用ソースボイス。</summary>
	IXAudio2SourceVoice* streamVoice_ = nullptr;
	/// <summary>ダブルバッファリング用バッファ。</summary>
	std::vector<std::vector<BYTE>> audioBuffers_;
	/// <summary>バッファサイズ。</summary>
	size_t BUFFER_SIZE;
	/// <summary>バッファ数。</summary>
	static constexpr int BUFFER_COUNT = 3;
	/// <summary>リバーブパラメータ。</summary>
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters_ = {};
	/// <summary>エフェクトチェーン設定。</summary>
	XAUDIO2_EFFECT_CHAIN effectChain_ = {};
	/// <summary>エフェクトディスクリプタ。</summary>
	XAUDIO2_EFFECT_DESCRIPTOR effect_[1] = {};

	/// <summary>コールバック。</summary>
	StreamingVoiceCallback callback_;
};

