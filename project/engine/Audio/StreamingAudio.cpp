#include "StreamingAudio.h"

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

	// バッファの準備（トリプルバッファリング）
	constexpr int BUFFER_COUNT = 3; 
	BUFFER_SIZE = header.sampleRate * waveFormat.nBlockAlign; // 1秒分のサイズ
	audioBuffers_.resize(BUFFER_COUNT, std::vector<BYTE>(BUFFER_SIZE));
	XAUDIO2_BUFFER xAudioBuffers[BUFFER_COUNT] = {};
	StreamingVoiceCallback callback;


	// ソースボイスを作成し、コールバックを登録
	if (FAILED(xAudio2_->CreateSourceVoice(&streamVoice_, &waveFormat, XAUDIO2_VOICE_USEFILTER, XAUDIO2_MAX_FREQ_RATIO, &callback, nullptr))) {
		Logger::Log("Failed to create source voice.\n");
		return;
	}

	InitEffectChain();


	// 再生開始
	streamVoice_->Start(0);
	
	int currentBufferIndex = 0;

	// ストリーミングループ（別スレッドで実行）
	// isStreaming_ が true であり続ける限りループ
	while (isStreaming_.load()) {
		std::vector<BYTE>& currentBuffer = audioBuffers_[currentBufferIndex];
		
		// 音声データの読み込み
		if (!ReadAudioData(audioFile, currentBuffer)) {
			// EOF到達時の処理
			if (isLoopStreaming_.load()) {
				// ループ再生：先頭に戻って再読み込み
				audioFile.clear();  // EOFフラグクリア
				audioFile.seekg(sizeof(WAVHeader), std::ios::beg);
				if (!ReadAudioData(audioFile, currentBuffer)) {
					break; // 読み込み失敗なら終了
				}
			}
			else if (!isLoopStreaming_.load()) {
				break; // ループなしなら終了
			}
			else {
				Logger::Log("Failed to ReadAudioData.\n");
				break;
			}
		}

		// XAudio2バッファの設定
		XAUDIO2_BUFFER& xBuffer = xAudioBuffers[currentBufferIndex];
		xBuffer.AudioBytes = static_cast<UINT32>(currentBuffer.size());
		xBuffer.pAudioData = currentBuffer.data();
		xBuffer.Flags = 0;

		// 最後のデータにはフラグを追加
		if (audioFile.eof()) {
			xBuffer.Flags = XAUDIO2_END_OF_STREAM;
		}

		// バッファをキューに送信
		if (FAILED(streamVoice_->SubmitSourceBuffer(&xBuffer))) {
			Logger::Log("Failed to submit buffer.\n");
			break;
		}

		Logger::Log("Buffer submitted.\n");

		// 次のバッファへ切り替え
		currentBufferIndex = (currentBufferIndex + 1) % BUFFER_COUNT;

		// 前のバッファの再生完了を待機（同期制御: コールバック活用）
		callback.WaitForBuffer();
	}

	// クリーンアップ
	audioFile.close();
	streamVoice_->Stop(0);
	streamVoice_->DestroyVoice();
	streamVoice_ = nullptr;
	Logger::Log("Streaming finished.\n");
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
