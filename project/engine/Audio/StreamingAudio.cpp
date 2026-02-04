#include "StreamingAudio.h"

namespace TYEngine {
namespace Audio {

using namespace TYEngine::Core;

uint32_t StreamingAudio::nextID_ = 0;

StreamingAudio::~StreamingAudio()
{
	StopStreaming();
	// BGMリソースの解放
	if (streamVoice_ != nullptr) {
		streamVoice_->DestroyVoice();
		streamVoice_ = nullptr;
	}
}

void StreamingAudio::StartStreaming(const char* filename, bool isLoop)
{
	// すでにストリーミング中の場合は一旦終了
	if (isStreaming_.load()) {
		StopStreaming();
	}
	if (isLoop) {
		isLoopStreaming_.store(true);
	}

	isStreaming_.store(true);

	// ストリーミングスレッドでストリーミング再生を開始
	audioThread_ = std::make_unique<std::thread>(&StreamingAudio::StreamAudio, this, filename);
}

void StreamingAudio::StopStreaming()
{
	if (isStreaming_.load()) {
		isStreaming_.store(false); // ストリーミングを停止するフラグをセット

		if (audioThread_ && audioThread_->joinable()) {
			audioThread_->join(); // スレッドが終了するのを待つ
			Logger::Log("Stop streaming thread.\n");
		}
	}
}

void StreamingAudio::SetPitch(float pitch)
{
	if (streamVoice_)
	{
		streamVoice_->SetFrequencyRatio(pitch);
	}
	else {
		Logger::Log("Uninitialized streamVoice.");
	}
}

void StreamingAudio::ApplyEffectChain()
{
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

void StreamingAudio::SetEffect(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS parameters)
{
	if (streamVoice_)
	{
		ReverbConvertI3DL2ToNative(&parameters, &reverbParameters_);
		if (FAILED(streamVoice_->SetEffectParameters(0, &reverbParameters_, sizeof(reverbParameters_)))) {
			Logger::Log("Failed to set effect parameters.");
		}
		streamVoice_->EnableEffect(0);
	}
	else {
		Logger::Log("Uninitialized streamVoice.");
	}
}

void StreamingAudio::DisableEffect()
{
	if (streamVoice_)
	{
		streamVoice_->DisableEffect(0);
	}
}

void StreamingAudio::StreamAudio(const char* filename)
{
	std::string filePath = directoryPath_;
	filePath += filename;

	// WAVヘッダーの解析と読み込み
	WAVHeader header;
	if (!ReadWavHeader(filePath, header)) {
		Logger::Log("Error reading WAV header.\n");
		return;
	}

	// WAVEFORMATEXの設定
	WAVEFORMATEX waveFormat = {};
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = header.numChannels;
	waveFormat.nSamplesPerSec = header.sampleRate;
	waveFormat.wBitsPerSample = header.bitsPerSample;
	waveFormat.nBlockAlign = header.numChannels * (header.bitsPerSample / 8);
	waveFormat.nAvgBytesPerSec = header.sampleRate * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0; // PCMでは0

	// ストリーミング用のファイルを開く
	std::ifstream audioFile(filePath, std::ios::binary);
	if (!audioFile) {
		Logger::Log("Error opening file.\n");
		return;
	}

	// データ部分へシーク
	audioFile.seekg(sizeof(WAVHeader));

	BUFFER_SIZE = header.sampleRate * waveFormat.nBlockAlign; // 1秒分のサイズ


	// ソースボイスを作成し、コールバックを登録
	if (FAILED(xAudio2_->CreateSourceVoice(&streamVoice_, &waveFormat, XAUDIO2_VOICE_USEFILTER, XAUDIO2_MAX_FREQ_RATIO, &callback_, nullptr))) {
		Logger::Log("Failed to create source voice.\n");
		return;
	}

	InitEffectChain();

	PlayStream(audioFile);

	// クリーンアップ
	audioFile.close();
	streamVoice_->Stop(0);
	streamVoice_->DestroyVoice();
	streamVoice_ = nullptr;
	Logger::Log("Streaming finished.\n");
}

void StreamingAudio::PlayStream(std::ifstream& audioFile)
{
	// 1. 状態の初期化
	callback_.Initialize(BUFFER_COUNT);
	audioBuffers_.assign(BUFFER_COUNT, std::vector<BYTE>(BUFFER_SIZE));
	int currentBufferIndex = 0;

	Logger::Log("Streaming loop started. BufferCount: " + std::to_string(BUFFER_COUNT) + "\n");

	// 2. 【初動】ループ前に3つ（BUFFER_COUNT分）すべてのバッファを先に送る
	for (int i = 0; i < BUFFER_COUNT; ++i)
	{
		if (!ReadAudioData(audioFile, audioBuffers_[currentBufferIndex])) break;

		XAUDIO2_BUFFER xBuffer = {};
		xBuffer.AudioBytes = static_cast<UINT32>(audioBuffers_[currentBufferIndex].size());
		xBuffer.pAudioData = audioBuffers_[currentBufferIndex].data();

		if (FAILED(streamVoice_->SubmitSourceBuffer(&xBuffer)))
		{
			Logger::Log("Initial submit failed at index " + std::to_string(currentBufferIndex) + "\n");
			return;
		}

		// 送ったので「空き」を1つ減らす
		callback_.ConsumeOneBuffer();

		Logger::Log("Initial buffer [" + std::to_string(currentBufferIndex) + "] pre-submitted.\n");
		currentBufferIndex = (currentBufferIndex + 1) % BUFFER_COUNT;
	}

	// 3. 再生開始
	streamVoice_->Start(0);

	// 4. 【メインループ】
	while (isStreaming_.load())
	{
		// 再生が1つ終わる（空きが出る）まで待機
		callback_.WaitForBuffer();

		std::vector<BYTE>& currentBuffer = audioBuffers_[currentBufferIndex];

		// データの読み込み
		if (!ReadAudioData(audioFile, currentBuffer))
		{
			if (isLoopStreaming_.load())
			{
				Logger::Log("Looping stream.\n");
				audioFile.clear();
				audioFile.seekg(sizeof(WAVHeader), std::ios::beg);
				if (!ReadAudioData(audioFile, currentBuffer)) break;
			}
			else
			{
				Logger::Log("End of stream reached.\n");
				break;
			}
		}

		XAUDIO2_BUFFER xBuffer = {};
		xBuffer.AudioBytes = static_cast<UINT32>(currentBuffer.size());
		xBuffer.pAudioData = currentBuffer.data();

		if (FAILED(streamVoice_->SubmitSourceBuffer(&xBuffer)))
		{
			Logger::Log("SubmitSourceBuffer failed.\n");
			break;
		}

		Logger::Log("Buffer [" + std::to_string(currentBufferIndex) + "] submitted.\n");
		currentBufferIndex = (currentBufferIndex + 1) % BUFFER_COUNT;
	}

	Logger::Log("Streaming loop finished.\n");
}

void StreamingAudio::InitEffectChain()
{
	// リバーブエフェクトを作成
	IUnknown* reverbEffect = nullptr;
	if (FAILED(XAudio2CreateReverb(&reverbEffect))) // Reverbエフェクトを作成
	{
		Logger::Log("Failed to create reverb effect.");
	}
	else {
		Logger::Log("Succeeded to create reverb effect.");
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

bool StreamingAudio::ReadAudioData(std::ifstream& file, std::vector<BYTE>& buffer)
{
	if (!file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
		return false; // EOF またはエラー
	}
	return true;
}

bool StreamingAudio::ReadWavHeader(std::string filename, WAVHeader& header)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file) return false;

	file.read(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
	return (header.riff[0] == 'R' && header.riff[1] == 'I' && header.riff[2] == 'F' && header.riff[3] == 'F') &&
		(header.wave[0] == 'W' && header.wave[1] == 'A' && header.wave[2] == 'V' && header.wave[3] == 'E') &&
		(header.fmt[0] == 'f' && header.fmt[1] == 'm' && header.fmt[2] == 't' && header.fmt[3] == ' ');
}

} // namespace Audio
} // namespace TYEngine
