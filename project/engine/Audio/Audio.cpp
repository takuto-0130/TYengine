#include "Audio.h"
#include <cassert>
#include <Logger.h>

Audio::~Audio()
{
	// BGMリソースの解放
	for (auto SourceVoice : sourceVoices_)
	{
		if (SourceVoice != nullptr)
		{
			SourceVoice->DestroyVoice();
			SourceVoice = nullptr;
		}
	}

	// カテゴリ用 SubmixVoices の破棄
	for (auto& [name, voice] : soundCategorySubmixVoices_)
	{
		if (voice)
		{
			voice->DestroyVoice();
		}
	}
	soundCategorySubmixVoices_.clear();

	// 解析用 SubmixVoice の破棄
	if (analyzerSubmix_)
	{
		analyzerSubmix_->DestroyVoice();
		analyzerSubmix_ = nullptr;
	}

	if (masterVoice_ != nullptr)
	{
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}

	soundDataMap_.clear();
	xAudio2_.Reset();
}

void Audio::Initialize(const std::string& directoryPath)
{
	directoryPath_ = directoryPath;
	HRESULT result;
	// インスタンスの生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	// マスターボイスの生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);

	if (FAILED(result))
	{
		// ログに警告を出す（デバッグ用）
		Logger::Log("Audio Warning: Failed to initialize audio client.\n");

		assert(SUCCEEDED(result));
	}

	CreateAnalyzerSubmix();
	Start();
}

void Audio::CreateAnalyzerSubmix()
{
	// ============================================
	//   ★ SubmixVoice を作る
	// ============================================

	// MasterVoice と同じフォーマットで作る
	XAUDIO2_VOICE_DETAILS details = {};
	masterVoice_->GetVoiceDetails(&details);

	HRESULT result;

	result = xAudio2_->CreateSubmixVoice(
		&analyzerSubmix_,
		details.InputChannels,
		details.InputSampleRate,
		0, 
		kAnalyzer // ProcessingStage (下から上へ)
	);
	assert(SUCCEEDED(result));

	// Submix → MasteringVoice へ音を送る
	XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
	sendDesc.Flags = 0;
	sendDesc.pOutputVoice = masterVoice_;

	XAUDIO2_VOICE_SENDS sends = {};
	sends.SendCount = 1;
	sends.pSends = &sendDesc;

	result = analyzerSubmix_->SetOutputVoices(&sends);
	assert(SUCCEEDED(result));

	// ============================================
	//   ★ SubmixVoice に XAPO を付ける
	// ============================================

	// XAPO インスタンス
	analyzerXAPO_ = Microsoft::WRL::Make<MyAnalyzerXAPO>();

	// Effect desc
	XAUDIO2_EFFECT_DESCRIPTOR effectDesc = {};
	effectDesc.InitialState = TRUE;
	effectDesc.OutputChannels = details.InputChannels;
	effectDesc.pEffect = static_cast<IXAPO*>(analyzerXAPO_.Get());

	// Effect chain
	XAUDIO2_EFFECT_CHAIN effectChainXAPO = {};
	effectChainXAPO.EffectCount = 1;
	effectChainXAPO.pEffectDescriptors = &effectDesc;

	// SubmixVoice に XAPO をセット
	result = analyzerSubmix_->SetEffectChain(&effectChainXAPO);
	assert(SUCCEEDED(result));
}

void Audio::Start()
{
	xAudio2_->StartEngine();

	XAUDIO2_VOICE_DETAILS details = {};
	analyzerSubmix_->GetVoiceDetails(&details);

	HRESULT result;

	// ---- ★無音バッファ用 Voice を作成 ----
	silentFormat_.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	silentFormat_.nChannels = 1;
	silentFormat_.nSamplesPerSec = details.InputSampleRate;
	silentFormat_.wBitsPerSample = 32;
	silentFormat_.nBlockAlign = 4;
	silentFormat_.nAvgBytesPerSec = details.InputSampleRate * 4;
	silentFormat_.cbSize = 0;

	// 無音バッファ（50ms程度）
	int silentSamples = int(details.InputSampleRate * 0.05f);
	silentBuffer_.assign(silentSamples, 0.0f);

	XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
	sendDesc.Flags = 0;
	sendDesc.pOutputVoice = analyzerSubmix_;

	XAUDIO2_VOICE_SENDS sends = {};
	sends.SendCount = 1;
	sends.pSends = &sendDesc;

	result = xAudio2_->CreateSourceVoice(
		&silentVoice_,
		&silentFormat_,
		0, 2.0f,
		nullptr, &sends, nullptr
	);

	assert(SUCCEEDED(result));

	silentVoice_->Start(0);
	EnableSilentFeed(true);
}

void Audio::Update()
{
	if (!silentFeedEnabled_) return;

	XAUDIO2_VOICE_STATE st{};
	silentVoice_->GetState(&st);

	if (st.BuffersQueued < 3)
	{
		XAUDIO2_BUFFER buf{};
		buf.AudioBytes = static_cast<UINT32>(silentBuffer_.size() * sizeof(float));
		buf.pAudioData = reinterpret_cast<const BYTE*>(silentBuffer_.data());

		silentVoice_->SubmitSourceBuffer(&buf);
	}
}

void Audio::EnableSilentFeed(bool enable)
{
	silentFeedEnabled_ = enable;

	if (enable) silentVoice_->Start();
	else silentVoice_->Stop();
}

void Audio::StopBGM(int resourceNum)
{
	sourceVoices_[resourceNum]->Stop();
	sourceVoices_[resourceNum]->FlushSourceBuffers();
}

void Audio::Pause(int resourceNum)
{
	sourceVoices_[resourceNum]->Stop();
}

void Audio::ReStart(int resourceNum)
{
	sourceVoices_[resourceNum]->Start();
}

void Audio::SetMasterVolume(float volume)
{
	masterVoice_->SetVolume(volume);
}

void Audio::SetCategoryVolume(const std::string& soundCategory, float volume)
{
	soundCategorySubmixVoices_[soundCategory]->SetVolume(volume);
}

void Audio::SetSoundVolume(int resourceNum, float volume)
{
	sourceVoices_[resourceNum]->SetVolume(volume);
}

float Audio::GetMasterVolume()
{
	float volume = 0;
	masterVoice_->GetVolume(&volume);
	return volume;
}

float Audio::GetCategoryVolume(const std::string& soundCategory)
{
	float volume = 0;
	soundCategorySubmixVoices_[soundCategory]->GetVolume(&volume);
	return volume;
}

float Audio::GetSoundVolume(int resourceNum)
{
	float volume = 0;
	sourceVoices_[resourceNum]->GetVolume(&volume);
	return volume;
}

void Audio::LoadWave(const std::string& filename)
{
	if (soundDataMap_.count(filename)) {
		// キーが存在する場合、処理を中断
		return;
	}

	// ファイル入力 stream のインスタンス
	std::ifstream file;
	std::string filePath = directoryPath_;
	filePath += filename;
	filePath += ".wav";
	file.open(filePath, std::ios_base::binary);
	assert(file.is_open());

	// .wavデータ読み込み
	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEがチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	// LISTチャンクを検出した場合
	if (strncmp(data.id, "LIST", 4) == 0) {
		// 読み取り位置をLISTチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	// INFOISFTチャンクを検出した場合
	if (strncmp(data.id, "INFO", 4) == 0) {
		// 読み取り位置をINFOISFTチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部 (波形のデータ) の読み込み
	// SoundDataの生成
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.playSoundLength = data.size / format.fmt.nBlockAlign;

	// vectorをリサイズして直接読み込む
	soundData.buffer.resize(data.size);
	file.read(reinterpret_cast<char*>(soundData.buffer.data()), data.size);


	// ファイルクローズ
	file.close();

	soundDataMap_[filename] = std::move(soundData);
}

void Audio::SoundUnload(const std::string& filename)
{
	if (soundDataMap_.erase(filename) == 0)
	{
		Logger::Log("Sound not loaded.\n");
	}
}

void Audio::AddSoundCategory(const std::string& soundCategory)
{
	if (soundCategorySubmixVoices_.count(soundCategory))
	{
		// キーが存在する場合、処理を中断
		return;
	}

	HRESULT result;

	// analyzerSubmix と同じフォーマットで作る
	XAUDIO2_VOICE_DETAILS details = {};
	analyzerSubmix_->GetVoiceDetails(&details);

	// ボイス
	IXAudio2SubmixVoice* voice;

	XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
	sendDesc.Flags = 0;
	sendDesc.pOutputVoice = analyzerSubmix_;

	XAUDIO2_VOICE_SENDS sends = {};
	sends.SendCount = 1;
	sends.pSends = &sendDesc;

	result = xAudio2_->CreateSubmixVoice(
		&voice,
		details.InputChannels,
		details.InputSampleRate,
		0,          
		kCategory,          // ProcessingStage (下から上へ)
		&sends,     
		nullptr
	);
	assert(SUCCEEDED(result));

	soundCategorySubmixVoices_[soundCategory] = voice;
}

int Audio::Play(const std::string& filename, const bool isLoop, std::string soundCategory)
{
	HRESULT result;

	auto it = soundDataMap_.find(filename);
	if (it == soundDataMap_.end())
	{
		Logger::Log("Sound not loaded.\n");
		return -1;
	}
	SoundData& soundData = it->second;

	// 今回使うサウンドデータ
	int sourceNum = -1;

	// 使用できるリソースを検索
	sourceNum = SearchSourceVoice(sourceVoices_.data());

	// 使用できるリソースがない場合は-1を返す
	if (sourceNum == -1) {
		Logger::Log("No sound resource available.\n"); 
		return -1; 
	}

	// 再生停止中、もしくは残りの再生数が最小のリソースを使用
	if (sourceVoices_[sourceNum] != nullptr)
	{
		sourceVoices_[sourceNum]->Stop();
		sourceVoices_[sourceNum]->FlushSourceBuffers(); 
		sourceVoices_[sourceNum]->DestroyVoice();
		sourceVoices_[sourceNum] = nullptr;
	}

	// 出力先をの Submix を決定
	XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
	sendDesc.Flags = 0;
	if(soundCategory == "")
	{
		// 指定ナシなら analyzer へ直接送る
		sendDesc.pOutputVoice = analyzerSubmix_;
	}
	else
	{
		// 指定アリなら捜査して送る
		auto itSub = soundCategorySubmixVoices_.find(soundCategory);
		if (itSub == soundCategorySubmixVoices_.end())
		{
			// カテゴリが見つからない場合のエラー処理
			Logger::Log("SoundCategory not found.\n");
			// analyzer へ直接送る
			sendDesc.pOutputVoice = analyzerSubmix_;
		}
		IXAudio2SubmixVoice* submix = itSub->second;

		sendDesc.pOutputVoice = submix;
	}

	XAUDIO2_VOICE_SENDS sends = {};
	sends.SendCount = 1;
	sends.pSends = &sendDesc;

	// 波形フォーマットをもとにSourceVoiceの生成
	if (FAILED(xAudio2_->CreateSourceVoice(&sourceVoices_[sourceNum],
		&soundData.wfex, 0,
		XAUDIO2_DEFAULT_FREQ_RATIO,
		nullptr,
		&sends)))
	{
		Logger::Log("Failed to create source voice.\n");
		return -1;
	}

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf = SetBuffer(isLoop, soundData);

	// 波形データの再生
	result = sourceVoices_[sourceNum]->SubmitSourceBuffer(&buf);
	result = sourceVoices_[sourceNum]->Start();

	return sourceNum;
}

int Audio::SearchSourceVoice(IXAudio2SourceVoice** sourceVoices)
{
	// 今回再生するリソース
	int sourceVoiceNum = -1;

	// リソースのバッファ
	unsigned int soundBuffer = 0;

	// 使用できる再生リソースを検索
	for (int i = 0; i < kMaxPlayWave; i++)
	{
		if (sourceVoices[i] == nullptr) {
			sourceVoiceNum = i;
			break;
		}
		// 現在の状態を取得
		XAUDIO2_VOICE_STATE state;
		sourceVoices[i]->GetState(&state);

		// バッファが0ならば再生可能と判断
		if (state.BuffersQueued == 0)
		{
			sourceVoiceNum = i;
			break;
		}
		else
		{
			// 初期値もしくはバッファが最小の場合は入れ替え
			if (soundBuffer == 0 || soundBuffer > state.BuffersQueued)
			{
				soundBuffer = state.BuffersQueued;
				sourceVoiceNum = i;
			}
		}
	}

	return sourceVoiceNum;
}

XAUDIO2_BUFFER Audio::SetBuffer(bool loop, const SoundData& sound)
{
	// バッファの初期化
	XAUDIO2_BUFFER buffer = {};

	// バッファ設定
	buffer.pAudioData = sound.buffer.data(); // vectorの先頭アドレス
	buffer.AudioBytes = static_cast<UINT32>(sound.buffer.size());
	buffer.PlayBegin = 0;
	buffer.PlayLength = sound.playSoundLength;

	// ループ設定
	if (loop)
	{
		buffer.LoopBegin = 0;
		buffer.LoopLength = sound.playSoundLength;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	return buffer;
}
